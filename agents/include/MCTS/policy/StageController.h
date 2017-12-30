#pragma once

#include <cstdint>

namespace mcts {
	namespace policy {
		class StageController {
		public:
			// Switch to simulation mode if node is chosen too few times
			// This can lower down the rate we allocate new nodes
			static constexpr int kSwitchToSimulationUnderChosenTimes = 10;

			static bool SwitchToSimulation(bool new_node_created, std::int64_t last_node_chosen_times) {
				if (new_node_created) return true;
				if (last_node_chosen_times < kSwitchToSimulationUnderChosenTimes) return true;
				return false;
			}
		};
	}
}
