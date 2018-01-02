#pragma once

#include <atomic>
#include <cstdint>


namespace mcts
{
	namespace selection
	{
		// Thread safety:
		//    Can be read from several threads concurrently
		//    Can only be write from one thread
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0), credit(0), total(0) {}

			void AddChosenTimes(int v) { chosen_times += v; }
			auto GetChosenTimes() const { return chosen_times.load(); }

			float GetAverageCredit() const;
			void AddCredit(float score, int repeat_times = 1);
			auto GetTotal() const { return total.load(); }

		private:
			std::atomic<std::int64_t> chosen_times;
			std::atomic<std::int64_t> credit;
			std::atomic<std::int64_t> total;
		};
	}
}