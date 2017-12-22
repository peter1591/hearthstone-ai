#pragma once

#include "engine/FlowControl/Manipulators/Helpers/DamageHelper.h"

namespace engine {
	namespace FlowControl
	{
		namespace enchantment { class Handler; }
		namespace Manipulators
		{
			namespace detail
			{
				class DamageSetter
				{
					friend class Helpers::DamageHelper;
					friend class Helpers::EnchantmentHelper;
					friend class FlowControl::enchantment::Handler;

				public:
					DamageSetter(state::State & state, state::CardRef card_ref) : state_(state), card_ref_(card_ref) {}

				private:
					void TakeDamage(int v, int* real_damage)
					{
						state::Cards::Card & card = state_.GetMutableCard(card_ref_);
						int armor = card.GetArmor();
						if (armor > 0) {
							if (v <= armor) {
								card.SetArmor(armor - v);
								*real_damage = 0;
								return;
							}
							card.SetArmor(0);
							v -= armor;
						}
						*real_damage = v;
						card.GetDamageSetter().Set(card.GetDamage() + v);
					}
					void Heal(int v)
					{
						state::Cards::Card & card = state_.GetMutableCard(card_ref_);
						int old_damage = card.GetDamage();
						int new_damage = old_damage - v;
						if (new_damage < 0) new_damage = 0;
						card.GetDamageSetter().Set(new_damage);
					}
					void SetAsUndamaged()
					{
						state::Cards::Card & card = state_.GetMutableCard(card_ref_);
						card.GetDamageSetter().Set(0);
					}

				private:
					state::State & state_;
					state::CardRef card_ref_;
				};
			}
		}
	}
}