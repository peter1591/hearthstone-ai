#pragma once

#include "MCTS/selection/EdgeAddon.h"

#include "MCTS/Config.h"

namespace mcts
{
	namespace selection
	{
		inline float EdgeAddon::GetAverageCredit() const {
			auto total_load = total.load();
			assert(total_load > 0);
			float ret = (float)credit.load() / total_load;
			assert(ret >= 0.0);
			assert(ret <= 1.0);
			return ret;
		}

		inline void EdgeAddon::AddCredit(float score, int repeat_times) {
			int total_increment = StaticConfigs::kCreditGranularity;
			int credit_increment = (int)(score * StaticConfigs::kCreditGranularity);

			// These two fields are not updated in an atomic operation. But this should be fine...
			total += total_increment * repeat_times;
			credit += credit_increment * repeat_times;
		}
	}
}