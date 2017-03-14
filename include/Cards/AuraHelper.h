#pragma once

#include "state/Cards/aura/AuraHandler.h"
#include "state/targetor/TargetsGenerator.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	// Emit when alive
	template <typename EnchantmentType>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
			: card_data_(card_data)
		{
			card_data_.aura_handler.is_valid = [](auto context) {
				return MinionCardUtils::IsAlive(context, context.card_ref_);
			};
		}

		~AuraHelper()
		{
			ApplyToAuraHandler();
		}

		AuraHelper & Target(state::Cards::aura::AuraHandler::FuncGetTargets* get_targets)
		{
			assert(get_targets != nullptr);
			card_data_.aura_handler.get_targets = get_targets;
			return *this;
		}

	private:
		void ApplyToAuraHandler()
		{
			assert(card_data_.aura_handler.get_targets != nullptr);

			card_data_.aura_handler.apply_on = [](auto context) {
				context.enchant_id_ = MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Add(EnchantmentType());
			};
			card_data_.aura_handler.remove_from = [](auto context) {
				MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Remove<EnchantmentType>(context.enchant_id_);
			};

			card_data_.added_to_play_zone += [](auto context) {
				state::CardRef self = context.card_ref_;
				context.state_.AddEvent<state::Events::EventTypes::UpdateAura>(
					[self](auto& controller, auto& context) {
					FlowControl::Manipulate(context.state_, context.flow_context_)
						.Card(self).Aura().Update();

					state::Cards::Card const& self_card = context.state_.GetCardsManager().Get(self);
					if (self_card.GetRawData().aura_aux_data.removed) {
						assert(self_card.GetRawData().aura_aux_data.Empty());
						controller.Remove();
					}
				});
			};
		}

	private:
		state::Cards::CardData & card_data_;
	};
}