#pragma once

#include <iostream>

#include "MCTS/Config.h"

namespace mcts
{
	template <bool enabled = mcts::StaticConfigs::enable_statistic>
	class Statistic {
	public:
		void ApplyActionSucceeded(bool is_simulation) {}
		void ApplyActionFailed(bool is_simulation) {}
		void PrintMessage() {}
	};

	template <> class Statistic<true>
	{
	public:
		Statistic() :
			iterate_success_(0), iterate_total_(0),
			apply_selection_action_success_(0), apply_selection_action_total_(0),
			apply_simulation_action_success_(0), apply_simulation_action_total_(0)
		{}

		void IterateSucceeded() {
			iterate_success_++;
			iterate_total_++;
		}
		void IterateFailed() {
			iterate_total_++;
		}

		void ApplyActionSucceeded(bool is_simulation) {
			if (is_simulation) return ApplySimulationActionSucceeded();
			else return ApplySelectionActionSucceeded();
		}
		void ApplyActionFailed(bool is_simulation) {
			if (is_simulation) return ApplySimulationActionFailed();
			else return ApplySelectionActionFailed();
		}

		void ApplySelectionActionSucceeded() {
			apply_selection_action_success_++;
			apply_selection_action_total_++;
		}
		void ApplySelectionActionFailed() {
			apply_selection_action_total_++;
		}

		void ApplySimulationActionSucceeded() {
			apply_simulation_action_success_++;
			apply_simulation_action_total_++;
		}
		void ApplySimulationActionFailed() {
			apply_simulation_action_total_++;
		}

		void PrintMessage() {
			std::cout << "Apply selection action success rate: ";
			PrintRate(apply_selection_action_success_, apply_selection_action_total_);
			std::cout << std::endl;

			std::cout << "Apply simulation action success rate: ";
			PrintRate(apply_simulation_action_success_, apply_simulation_action_total_);
			std::cout << std::endl;

			std::cout << "Iterate success rate: ";
			PrintRate(iterate_success_, iterate_total_);
			std::cout << std::endl;
		}

	private:
		void PrintRate(int success, int total) {
			double rate = 0.0;
			if (total > 0) {
				rate = (int)((double)success * 100 / total);
			}
			std::cout << success << " / " << total << " (" << rate << "%)";
		}

	private:
		int iterate_success_;
		int iterate_total_;
		int apply_selection_action_success_;
		int apply_selection_action_total_;
		int apply_simulation_action_success_;
		int apply_simulation_action_total_;
	};
}