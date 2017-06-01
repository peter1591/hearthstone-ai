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
			void Silence() {
				// Remove all enchantments, including the aura enchantments coming from other minions.
				// Those aura enchantments will be added back in the next AuraUpdate()
				Clear();

				origin_states.RestoreToSilenceDefault();
			}

		public:
			template <typename EnchantmentType> auto PushBackNormalEnchantment(state::State const& state, EnchantmentType&& enchantment) {
				return enchantments.PushBackNormalEnchantment<EnchantmentType>(state, std::forward<EnchantmentType>(enchantment));
			}
			template <typename EnchantmentType> auto PushBackAuraEnchantment(state::State const& state, EnchantmentType&& enchantment) {
				return enchantments.PushBackAuraEnchantment<EnchantmentType>(std::forward<EnchantmentType>(enchantment));
			}
			template <typename EnchantmentType> auto PushBackEventHookedEnchantment(
				FlowControl::Manipulate const& manipulate, state::CardRef card_ref,
				EnchantmentType&& enchant, enchantment::Enchantments::EventHookedEnchantment::AuxData const& aux_data)
			{
				return enchantments.PushBackEventHookedEnchantment(
					manipulate, card_ref, std::forward<EnchantmentType>(enchant), aux_data);
			}

			bool Exists(TieredEnchantments::IdentifierType id) const { return enchantments.Exists(id); }

			void Clear() { enchantments.Clear(); }
			bool Empty() { return enchantments.Empty(); }
			void AfterCopied(FlowControl::Manipulate const& manipulate, state::CardRef card_ref) { enchantments.AfterCopied(manipulate, card_ref); }
			void Remove(TieredEnchantments::IdentifierType id) { return enchantments.Remove(id); }

			void Update(state::State & state, FlowContext & flow_context, state::CardRef card_ref, bool allow_hp_reduce);

		private:
			void UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states);
			void UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states);
			void UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states);

			void UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states);
			void UpdateCard(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states);

		private:
			state::Cards::EnchantableStates origin_states;
			TieredEnchantments enchantments;
		};
	}
}