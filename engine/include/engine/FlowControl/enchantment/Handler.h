#pragma once

#include "state/Cards/EnchantableStates.h"
#include "engine/FlowControl/enchantment/TieredEnchantments.h"

namespace engine {
	namespace FlowControl
	{
		class Manipulate;

		namespace enchantment
		{
			class Handler
			{
			public:
				Handler() : base_origin_states(nullptr), origin_states(), enchantments() {}

				Handler(Handler const& rhs) :
					base_origin_states(rhs.base_origin_states),
					origin_states(rhs.origin_states),
					enchantments(rhs.enchantments)
				{}
				Handler & operator=(Handler const& rhs) {
					base_origin_states = rhs.base_origin_states;
					origin_states = rhs.origin_states;
					enchantments = rhs.enchantments;
					return *this;
				}

				void RefCopy(Handler const& base) {
					assert(base.base_origin_states == nullptr);
					base_origin_states = &base.origin_states;
					enchantments.RefCopy(base.enchantments);
				}

				state::Cards::EnchantableStates const& GetOriginalStates() const {
					if (base_origin_states) return *base_origin_states;
					else return origin_states;
				}

				state::Cards::EnchantableStates & GetMutableOriginalStates() {
					if (base_origin_states) {
						origin_states = *base_origin_states; // copy on write
						base_origin_states = nullptr;
					}
					return origin_states;
				}

				void SetOriginalStates(state::Cards::EnchantableStates states) {
					if (base_origin_states) base_origin_states = nullptr;
					origin_states = states;
				}

				void Silence() {
					// Remove all enchantments, including the aura enchantments coming from other minions.
					// Those aura enchantments will be added back in the next AuraUpdate()
					Clear();

					GetMutableOriginalStates().RestoreToSilenceDefault();
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
					EnchantmentType&& enchant, Enchantments::EventHookedEnchantment::AuxData const& aux_data)
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
				state::Cards::EnchantableStates const* base_origin_states;
				state::Cards::EnchantableStates origin_states;

				TieredEnchantments enchantments;
			};
		}
	}
}