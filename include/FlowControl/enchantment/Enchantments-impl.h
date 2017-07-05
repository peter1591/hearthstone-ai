#pragma once

#include "FlowControl/enchantment/Enchantments.h"

#include "state/State.h"
#include "FlowControl/enchantment/detail/UpdateDecider-impl.h"

namespace FlowControl {
	namespace enchantment {
		inline bool Enchantments::NormalEnchantment::Apply(ApplyFunctorContext const& context) const {
			if (valid_until_turn > 0) {
				if (context.state_.GetTurn() > valid_until_turn) {
					return false;
				}
			}
			apply_functor(context);
			return true;
		}

		template<typename EnchantmentType_>
		inline void Enchantments::PushBackNormalEnchantment(state::State const & state, EnchantmentType_ && item)
		{
			update_decider_.AddItem();
			int valid_until_turn = -1;
			if (item.valid_this_turn) valid_until_turn = state.GetTurn();
			update_decider_.AddValidUntilTurn(valid_until_turn);

			if (item.force_update_every_time) update_decider_.AddForceUpdateItem();

			assert(item.apply_functor);
			enchantments_.PushBack(NormalEnchantment(item.apply_functor, valid_until_turn, item.force_update_every_time));
		}

		template<typename EnchantmentType_>
		inline Enchantments::IdentifierType Enchantments::PushBackEventHookedEnchantment(
			FlowControl::Manipulate const & manipulate,
			state::CardRef card_ref,
			EnchantmentType_ && item,
			enchantment::Enchantments::EventHookedEnchantment::AuxData const & aux_data)
		{
			assert(item.apply_functor);
			assert(item.register_functor);
			IdentifierType id = enchantments_.PushBack(EventHookedEnchantment(item.apply_functor, item.register_functor, aux_data));
			item.register_functor(manipulate, card_ref, id,
				std::get<EventHookedEnchantment>(*enchantments_.Get(id)).aux_data);

			update_decider_.AddItem();

			return id;
		}
	}
}