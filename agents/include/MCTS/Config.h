#pragma once

#include "MCTS/policy/SideController.h"
#include "MCTS/policy/StageController.h"
#include "MCTS/policy/CreditPolicy.h"
#include "MCTS/policy/RandomByRand.h"
#include "MCTS/policy/Selection.h"
#include "MCTS/policy/Simulation.h"

namespace mcts
{
	struct StaticConfigs
	{
		static constexpr bool enable_statistic = true; // TODO: disable for release builds

		using SideController = policy::SideController;
		using StageController = policy::StageController;

		using SelectionPhaseRandomActionPolicy = policy::RandomByMt19937;
		using SelectionPhaseSelectActionPolicy = policy::selection::UCBPolicy;
		static constexpr int kVirtualLoss = 3;

		using SimulationPhaseRandomActionPolicy = policy::RandomByMt19937;
		using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayouts;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayoutWithHardCodedRules;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::HeuristicPlayoutWithHeuristicEarlyCutoffPolicy;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::HardCodedPlayoutWithHeuristicEarlyCutoffPolicy;

		using CreditPolicy = policy::CreditPolicy;
	};
}
