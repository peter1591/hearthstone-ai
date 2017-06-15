#pragma once

#include "MCTS/randoms/RandomByRand.h"
#include "MCTS/selection/policy/RandomPolicy.h"
#include "MCTS/simulation/policy/RandomPolicy.h"

namespace mcts
{
	struct StaticConfigs
	{
		static constexpr bool enable_statistic = true; // TODO: disable for release builds

		using SelectionPhaseRandomActionPolicy = randoms::RandomByRand;
		using SelectionPhaseSelectActionPolicy = selection::policy::RandomPolicy; // TODO: use a strong policy

		using SimulationPhaseRandomActionPolicy = randoms::RandomByRand;
		using SimulationPhaseSelectActionPolicy = simulation::policy::RandomPolicy; // TODO: use a strong policy
	};
}