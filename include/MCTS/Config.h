#pragma once

#include "MCTS/policy/CreditPolicy.h"
#include "MCTS/policy/RandomByRand.h"
#include "MCTS/policy/Selection.h"
#include "MCTS/policy/Simulation.h"

namespace mcts
{
	struct StaticConfigs
	{
		static constexpr bool enable_statistic = true; // TODO: disable for release builds

		using SelectionPhaseRandomActionPolicy = policy::RandomByMt19937;
		using SelectionPhaseSelectActionPolicy = policy::selection::UCBPolicy;
		static constexpr int kVirtualLoss = 3;

		// Switch to simulation mode if node is chosen too few times
		// This can lower down the rate we allocate new nodes
		static constexpr int kSwitchToSimulationUnderChosenTimes = 1;

		using SimulationPhaseRandomActionPolicy = policy::RandomByMt19937;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayouts;
		using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayoutWithHardCodedRules;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::HardCodedPlayoutWithHeuristicEarlyCutoffPolicy;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayoutWithHeuristicEarlyCutoffPolicy;

		using CreditPolicy = policy::CreditPolicy;
	};
}
