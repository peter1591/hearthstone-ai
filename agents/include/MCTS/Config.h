#pragma once

#include <type_traits>

namespace mcts
{
	namespace policy {
		class SideController;
		class StageController;
		class CreditPolicy;
		class RandomByMt19937;

		namespace selection {
			class UCBPolicy;
		}

		namespace simulation {
			class RandomPlayouts;
			class RandomPlayoutWithHardCodedRules;
			class HeuristicPlayoutWithHeuristicEarlyCutoffPolicy;
			class HardCodedPlayoutWithHeuristicEarlyCutoffPolicy;
		}
	}

	namespace StaticConfigs
	{
		static constexpr bool enable_statistic = true; // TODO: disable for release builds

		using SideController = policy::SideController;
		using StageController = policy::StageController;

		using CreditPolicy = policy::CreditPolicy;
		static constexpr int kCreditGranularity = 100; // this effctively increase the 'total' field by 100 for each simulation

		namespace updater_policy {
			struct TreeUpdate {};
			struct LinearUpdate {};
		}
		using UpdaterPolicy = updater_policy::TreeUpdate;

		using SelectionPhaseRandomActionPolicy = policy::RandomByMt19937;
		using SelectionPhaseSelectActionPolicy = policy::selection::UCBPolicy;
		static constexpr int kVirtualLoss = 3;
		static constexpr bool kRecordLeadingNodes = std::is_same_v<UpdaterPolicy, updater_policy::TreeUpdate>;

		using SimulationPhaseRandomActionPolicy = policy::RandomByMt19937;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayouts;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::RandomPlayoutWithHardCodedRules;
		using SimulationPhaseSelectActionPolicy = policy::simulation::HeuristicPlayoutWithHeuristicEarlyCutoffPolicy;
		//using SimulationPhaseSelectActionPolicy = policy::simulation::HardCodedPlayoutWithHeuristicEarlyCutoffPolicy;
	};
}
