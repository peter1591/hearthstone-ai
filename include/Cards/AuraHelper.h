#pragma once

#include "state/Cards/aura/AuraHandler.h"
#include "state/targetor/TargetsGenerator.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	struct EmitWhenAlive {
		static void RegisterAura(state::Cards::CardData &card_data) {
			card_data.added_to_play_zone += [](auto context) {
				context.state_.GetAuraManager().Add(context.card_ref_);
			};
		}

		template <typename Context>
		static bool ShouldEmit(Context&& context, state::CardRef card_ref) {
			return MinionCardUtils::IsAlive(std::move(context), card_ref);
		}
	};

	struct EmitWhenInHand {
		// TODO: implement
	};

	template <typename EnchantmentType, typename EmitPolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data) : card_data_(card_data)
		{
			card_data_.aura_handler.is_valid = [](auto context) {
				return EmitPolicy::ShouldEmit(std::move(context), context.card_ref_);
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

			EmitPolicy::RegisterAura(card_data_);
		}

	private:
		state::Cards::CardData & card_data_;
	};
}