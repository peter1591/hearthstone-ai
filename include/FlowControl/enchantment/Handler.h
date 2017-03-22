#pragma once

#include "state/Cards/EnchantableStates.h"
#include "FlowControl/enchantment/TieredEnchantments.h"
#include "FlowControl/enchantment/AuraEnchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		class Handler
		{
		public:
			void SetOriginalStates(state::Cards::EnchantableStates states) { origin_states = states; }

		public:
			auto& Aura() { return aura_enchantments; }
			auto const& Aura() const { return aura_enchantments; }

		public:
			template <typename EnchantmentType> auto Add(state::State const& state) {
				return enchantments.PushBack<EnchantmentType>(state);
			}

			void Clear() {
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
			AuraEnchantments aura_enchantments;
		};
	}
}