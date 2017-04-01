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
			state::Cards::EnchantableStates const& GetOriginalStates() const { return origin_states; }
			void SetOriginalStates(state::Cards::EnchantableStates states) { origin_states = states; }

		public:
			template <typename EnchantmentType> auto PushBackNormalEnchantment(state::State const& state) {
				return enchantments.PushBackNormalEnchantment<EnchantmentType>(state);
			}
			template <typename EnchantmentType> auto PushBackAuraEnchantment(state::State const& state) {
				return enchantments.PushBackAuraEnchantment<EnchantmentType>();
			}

			bool Exists(TieredEnchantments::IdentifierType id) const { return enchantments.Exists(id); }

			void Clear() { enchantments.Clear(); }
			void AfterCopied() { enchantments.AfterCopied(); }
			void Remove(TieredEnchantments::IdentifierType id) { return enchantments.Remove(id); }

			void Update(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, bool allow_death);

		private:
			void UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);
			void UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);
			void UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);

			void UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states);

		private:
			state::Cards::EnchantableStates origin_states;
			TieredEnchantments enchantments;
		};
	}
}