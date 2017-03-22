#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"

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
				friend class FlowControl::enchantment::Handler;

			public:
				DamageSetter(state::Cards::Card & card) : card_(card) {}

			private:
				void TakeDamage(int v, int* real_damage)
				{
					int armor = card_.GetArmor();
					if (armor > 0) {
						if (v <= armor) {
							card_.SetArmor(armor - v);
							*real_damage = 0;
							return;
						}
						card_.SetArmor(0);
						v -= armor;
					}
					*real_damage = v;
					card_.GetDamageSetter().Set(card_.GetDamage() + v);
				}
				void Heal(int v)
				{
					int old_damage = card_.GetDamage();
					int new_damage = old_damage - v;
					if (new_damage < 0) new_damage = 0;
					card_.GetDamageSetter().Set(new_damage);
				}

			private:
				state::Cards::Card & card_;
			};
		}
	}
}