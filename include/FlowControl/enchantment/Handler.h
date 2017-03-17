#pragma once

#include "state/Cards/EnchantableStates.h"
#include "FlowControl/enchantment/TieredEnchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		class Handler
		{
		public:
			Handler() : need_update(true) {}

			void SetOriginalStates(state::Cards::EnchantableStates states) { origin_states = states; }

		public:
			bool Exists(TieredEnchantments::IdentifierType id) const
			{
				return enchantments.Exists(id);
			}

			template <typename EnchantmentType> auto Add(EnchantmentType&& enchantment) {
				need_update = true;
				return enchantments.PushBack(std::forward<EnchantmentType>(enchantment));
			}

			auto Remove(TieredEnchantments::IdentifierType id) {
				need_update = true;
				return enchantments.Remove(id);
			}

			void Clear() {
				need_update = true;
				enchantments.Clear();
			}

			void Update(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);

		private:
			void UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);
			void UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);
			void UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);

			void UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);

		private:
			state::Cards::EnchantableStates origin_states;
			TieredEnchantments enchantments;
			bool need_update;
		};
	}
}