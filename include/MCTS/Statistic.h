#pragma once

#include <iostream>

#include "MCTS/Config.h"

namespace mcts
{
	template <bool enabled = mcts::StaticConfigs::enable_statistic>
	class Statistic {
	public:
		void ApplyActionSucceeded() {}
		void ApplyActionFailed() {}
		void PrintMessage() {}
	};

	template <> class Statistic<true>
	{
	public:
		Statistic() :
			apply_action_success_(0), apply_action_total_(0)
		{}

		void ApplyActionSucceeded() {
			apply_action_success_++;
			apply_action_total_++;
		}
		void ApplyActionFailed() {
			apply_action_total_++;
		}

		void PrintMessage() {
			double apply_action_success_rate = 0.0;
			if (apply_action_total_ > 0) {
				apply_action_success_rate = (int)(apply_action_success_ * 100 / apply_action_total_);
			}

			std::cout << "Apply action success rate: "
				<< apply_action_success_ << " / " << apply_action_total_
				<< " (" << apply_action_success_rate << "%) "
				<< std::endl;
		}

	private:
		int apply_action_success_;
		int apply_action_total_;
	};
}