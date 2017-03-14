#pragma once

#include "state/State.h"

namespace Cards
{
	struct InvalidIfNoTarget {};
	struct ValidIfNoTarget {};

	template <typename TargetGetter>
	class BattlecryHelper
	{
	public:
		template <typename ApplyFunctor>
		BattlecryHelper(state::Cards::CardData & card_data, ApplyFunctor&& apply_functor) {
			card_data.battlecry_target_getter = [](auto context) {
				TargetGetter::GetBattleTargets(context.targets_generator_);
				return true;
			};
			card_data.battlecry = std::move(apply_functor);
		}
	};
}