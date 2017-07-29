#pragma once

#include <atomic>
#include <stdint.h>

namespace mcts
{
	namespace selection
	{
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0), credit(0), total(0) {}

			void AddChosenTimes(int v) { chosen_times += v; }
			auto GetChosenTimes() const { return chosen_times.load(); }

			void AddTotal(int v) { total += v; }
			auto GetTotal() const { return total.load(); }

			void AddCredit(int v) { credit += v; }
			auto GetCredit() const { return credit.load(); }

		private:
			std::atomic<std::uint64_t> chosen_times;

			// for win-rate
			std::atomic<std::uint64_t> credit;
			std::atomic<std::uint64_t> total;
		};
	}
}