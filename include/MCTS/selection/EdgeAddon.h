#pragma once

namespace mcts
{
	namespace selection
	{
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0), credit(0), total(0) {}

			int chosen_times;

			// for win-rate
			int credit;
			int total;
		};
	}
}