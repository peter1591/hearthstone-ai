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

			template <typename... Ts>
			auto GetSpecifiedTarget(Ts&&... args) { return getter_.GetSpecifiedTarget(std::forward<Ts>(args)...); }

			void Clear()
			{
				minion_put_location_ = -1;
			}

		private:
			IActionParameterGetter & getter_;
			int minion_put_location_;
		};
	}
}