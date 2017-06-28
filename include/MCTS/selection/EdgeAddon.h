#pragma once

namespace mcts
{
	namespace selection
	{
		class EdgeAddon
		{
		public:
			EdgeAddon() : chosen_times(0) {}

			int chosen_times;
		};
	}
}