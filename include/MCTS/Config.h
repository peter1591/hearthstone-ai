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

		using SimulationPhaseRandomActionPolicy = policy::RandomByMt19937;
		using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPolicy; // TODO: use a strong policy

		using CreditPolicy = policy::CreditPolicy;
	};
}