#pragma once

#include "Cards/TargetorUtils.h"
#include "State/Cards/AuraHandler.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	// Emit when alive
	template <typename EnchantmentType>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
			: card_data_(card_data), targetor_getter_(nullptr)
		{
		}

		~AuraHelper()
		{
			assert(targetor_getter_);
			ApplyToAuraHandler();
		}

		AuraHelper & Target(state::Cards::AuraHandler::FuncGetTargetorHelper * targetor_getter)
		{
			targetor_getter_ = targetor_getter;
			return *this;
		}

	private:
		void ApplyToAuraHandler()
		{
			card_data_.aura_handler.get_targetor_helper = targetor_getter_;
			card_data_.aura_handler.get_targets = [](auto& context) {
				if (!MinionCardUtils::IsAlive(context, context.card_ref_)) return;
				Cards::TargetorHelper targetor;
				(*context.card_.GetRawData().aura_handler.get_targetor_helper)(context, targetor);
				targetor.GetInfo().FillTargets(context.state_, context.targets_);
			};
			card_data_.aura_handler.apply_on = [](auto& context) {
				context.enchant_id_ = MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Add(EnchantmentType());
			};
			card_data_.aura_handler.remove_from = [](auto& context) {
				MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Remove<EnchantmentType>(context.enchant_id_);
			};
		}

	private:
		state::Cards::CardData & card_data_;
		state::Cards::AuraHandler::FuncGetTargetorHelper * targetor_getter_;
	};
}