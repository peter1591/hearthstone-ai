#pragma once

#include "state/IActionParameterGetter.h"
#include "state/TargetorInfo.h"
#include "Cards/TargetorInfoGetter.h"

namespace state {
	class ActionParameterWrapper
	{
	public:
		ActionParameterWrapper(IActionParameterGetter & getter)
			: getter_(getter), minion_put_location_(-1)
		{
		}

		ActionParameterWrapper(ActionParameterWrapper const&) = delete;
		ActionParameterWrapper& operator=(ActionParameterWrapper const&) = delete;

		int GetMinionPutLocation(int min, int max)
		{
			if (minion_put_location_ < 0) {
				if (min >= max) {
					minion_put_location_ = min;
				}
				else {
					minion_put_location_ = getter_.GetMinionPutLocation(min, max);
				}
			}
			return minion_put_location_;
		}

		CardRef GetBattlecryTarget(State & state, CardRef card_ref, const Cards::Card & card, TargetorInfo const& target_info)
		{
			if (!battlecry_target_.IsValid()) {
				std::vector<CardRef> targets;
				target_info.FillTargets(state, targets);
				battlecry_target_ = getter_.GetBattlecryTarget(state, card_ref, card, targets);
			}
			return battlecry_target_;
		}

		void Clear()
		{
			minion_put_location_ = -1;
			battlecry_target_.Invalidate();
		}

	private:
		IActionParameterGetter & getter_;
		int minion_put_location_;
		CardRef battlecry_target_;
	};
}