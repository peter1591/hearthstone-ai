#pragma once

#include "state/targetor/Targets.h"
#include "FlowControl/IActionParameterGetter.h"

namespace FlowControl {
	namespace detail {
		class ActionParameterWrapper
		{
		public:
			ActionParameterWrapper(IActionParameterGetter & getter)
				: getter_(getter), minion_put_location_(-1)
			{
			}

			ActionParameterWrapper(ActionParameterWrapper const&) = default;
			ActionParameterWrapper& operator=(ActionParameterWrapper const&) = default;
			ActionParameterWrapper(ActionParameterWrapper &&) = default;
			ActionParameterWrapper& operator=(ActionParameterWrapper &&) = default;

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
				if (!battlecry_target_.IsValid()) { // TODO: move this information to flow context
					std::vector<state::CardRef> targets;
					target_info.Fill(state, targets);
					battlecry_target_ = getter_.GetBattlecryTarget(state, card_ref, card, targets);
				}
				return battlecry_target_;
			}

			template <typename... Ts>
			auto GetSpecifiedTarget(Ts&&... args) { return getter_.GetBattlecryTarget(std::forward<Ts>(args)...); } // TODO: rename

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
}