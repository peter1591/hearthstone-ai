#pragma once

namespace FlowControl
{
	namespace Helpers
	{
		template <class ActionParameterGetter>
		class ActionParameterWrapper
		{
		public:
			ActionParameterWrapper(ActionParameterGetter & getter)
				: getter_(getter), minion_put_location_(-1)
			{
			}

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

			CardRef GetBattlecryTarget(state::State & state, CardRef card_ref, const state::Cards::Card & card)
			{
				if (!battlecry_target_.IsValid()) {
					battlecry_target_ = getter_.GetBattlecryTarget(state, card_ref, card);
				}
				return battlecry_target_;
			}

		private:
			ActionParameterGetter & getter_;
			int minion_put_location_;
			CardRef battlecry_target_;
		};
	}
}