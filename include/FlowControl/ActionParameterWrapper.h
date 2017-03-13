#pragma once

#include "state/targetor/Targets.h"
#include "FlowControl/IActionParameterGetter.h"

namespace FlowControl {
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

		state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, state::targetor::Targets const& target_info)
		{
			if (!battlecry_target_.IsValid()) {
				std::vector<state::CardRef> targets;
				target_info.Fill(state, targets);
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
		state::CardRef battlecry_target_;
	};
}