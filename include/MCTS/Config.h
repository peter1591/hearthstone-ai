#pragma once

#include "MCTS/policy/CreditPolicy.h"
#include "MCTS/policy/RandomByRand.h"
#include "MCTS/selection/policy/RandomPolicy.h"
#include "MCTS/simulation/policy/RandomPolicy.h"

namespace mcts
{
	struct StaticConfigs
	{
		static constexpr bool enable_statistic = true; // TODO: disable for release builds

		using SelectionPhaseRandomActionPolicy = policy::RandomByRand;
		using SelectionPhaseSelectActionPolicy = selection::policy::RandomPolicy; // TODO: use a strong policy

		using SimulationPhaseRandomActionPolicy = policy::RandomByRand;
		using SimulationPhaseSelectActionPolicy = simulation::policy::RandomPolicy; // TODO: use a strong policy

		using CreditPolicy = policy::CreditPolicy;
	};
}