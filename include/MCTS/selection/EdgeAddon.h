#pragma once

#include <stdint.h>

namespace mcts
{
	namespace selection
	{
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0), credit(0), total(0) {}

			std::uint64_t chosen_times;

			// for win-rate
			std::uint64_t credit;
			std::uint64_t total;
		};
	}
}