#pragma once

#include <atomic>
#include <cstdint>

#include "MCTS/Config.h"

namespace mcts
{
	namespace selection
	{
		// Thread safe
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0), credit(0), total(0) {}

			void AddChosenTimes(int v) { chosen_times += v; }
			auto GetChosenTimes() const { return chosen_times.load(); }

			float GetAverageCredit() const {
				auto total_load = total.load();
				assert(total_load > 0);
				float ret = (float)credit.load() / total_load;
				assert(ret >= -1.0);
				assert(ret <= 1.0);
				return ret;
			}

			void AddCredit(float score, int repeat_times = 1) {
				int total_increment = StaticConfigs::kCreditGranularity;
				int credit_increment = (int)(score * StaticConfigs::kCreditGranularity);

				assert(credit_increment >= -total_increment);
				assert(credit_increment <= total_increment);

				// These two fields are not updated in an atomic operation. But this should be fine...
				total += total_increment * repeat_times;
				credit += credit_increment * repeat_times;
			}
			
			auto GetTotal() const { return total.load(); }

		private:
			std::atomic<std::int64_t> chosen_times;
			std::atomic<std::int64_t> credit;
			std::atomic<std::int64_t> total;
		};
	}
}