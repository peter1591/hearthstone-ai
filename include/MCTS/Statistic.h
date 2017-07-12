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
			apply_selection_action_success_(0), apply_selection_action_total_(0),
			apply_simulation_action_success_(0), apply_simulation_action_total_(0)
		{}

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
			double apply_selection_action_success_rate = 0.0;
			if (apply_selection_action_total_ > 0) {
				apply_selection_action_success_rate =
					(int)((double)apply_selection_action_success_ * 100 / apply_selection_action_total_);
			}

			std::cout << "Apply selection action success rate: "
				<< apply_selection_action_success_ << " / " << apply_selection_action_total_
				<< " (" << apply_selection_action_success_rate << "%) "
				<< std::endl;

			double apply_simulation_action_success_rate = 0.0;
			if (apply_simulation_action_total_ > 0) {
				apply_simulation_action_success_rate =
					(int)((double)apply_simulation_action_success_ * 100 / apply_simulation_action_total_);
			}

			std::cout << "Apply simulation action success rate: "
				<< apply_simulation_action_success_ << " / " << apply_simulation_action_total_
				<< " (" << apply_simulation_action_success_rate << "%) "
				<< std::endl;
		}

	private:
		int apply_selection_action_success_;
		int apply_selection_action_total_;
		int apply_simulation_action_success_;
		int apply_simulation_action_total_;
	};
}