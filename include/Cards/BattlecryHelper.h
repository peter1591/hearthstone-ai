#pragma once

#include "state/State.h"

namespace Cards
{
	struct WithTargets {};
	struct NoTarget {};

	template <typename TargetsPolicy, typename TargetGetter> class BattlecryHelper;

	template <typename TargetGetter>
	class BattlecryHelper<WithTargets, TargetGetter> {
	public:
		template <typename ApplyFunctor>
		BattlecryHelper(state::Cards::CardData & card_data, ApplyFunctor&& apply_functor) {
			card_data.battlecry_target_getter = [](auto context) {
				TargetGetter::GetBattlecryTargets(context.targets_generator_, context);
				return true;
			};
			card_data.battlecry = std::move(apply_functor);
		}
	};

	template <typename TargetGetter>
	class BattlecryHelper<NoTarget, TargetGetter> {
	public:
		template <typename ApplyFunctor>
		BattlecryHelper(state::Cards::CardData & card_data, ApplyFunctor&& apply_functor) {
			card_data.battlecry = std::move(apply_functor);
		}
	};
}