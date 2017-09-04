#pragma once

#include <atomic>
#include <iostream>

#include "MCTS/Config.h"

namespace mcts
{
	// Thread safety: Yes
	template <bool enabled = mcts::StaticConfigs::enable_statistic>
	class Statistic {
	public:
		void ApplyActionSucceeded(bool is_simulation) {}
		void PrintMessage() {}
	};

	namespace detail {
		class SuccessRateRecorder {
		public:
			SuccessRateRecorder() : success_(0), total_(0) {}

			void ReportSuccess() {
				++success_;
				++total_;
			}
			void ReportFailed() {
				++total_;
			}

			int GetSuccessCount() const { return success_; }
			int GetTotalCount() const { return total_; }

			double GetSuccessRate() const {
				if (total_ == 0) return 0.0;
				return (double)success_ / total_;
			}

		private:
			std::atomic<int> success_;
			std::atomic<int> total_;
		};
	}

	// Thread safety: Yes
	template <> class Statistic<true>
	{
	public:
		Statistic() : iterate_(), selection_(), simulation_() {}

		void IterateSucceeded() { iterate_.ReportSuccess(); }
		void IterateFailed() { iterate_.ReportFailed(); }
		auto GetSuccededIterates() const { return iterate_.GetSuccessCount(); }

		void ApplyActionSucceeded(bool is_simulation) {
			if (is_simulation) return simulation_.ReportSuccess();
			else return selection_.ReportSuccess();
		}

		void ApplySelectionActionSucceeded() {
			selection_.ReportSuccess();
		}

		void ApplySimulationActionSucceeded() {
			simulation_.ReportSuccess();
		}

		void PrintMessage() const {
			std::cout << "Apply selection action success rate: ";
			PrintRate(selection_);
			std::cout << std::endl;

			std::cout << "Apply simulation action success rate: ";
			PrintRate(simulation_);
			std::cout << std::endl;

			std::cout << "Iterate success rate: ";
			PrintRate(iterate_);
			std::cout << std::endl;
		}

	private:
		void PrintRate(detail::SuccessRateRecorder const& rate) const {
			std::cout << rate.GetSuccessCount()
				<< " / " << rate.GetTotalCount()
				<< " (" << 100.0 * rate.GetSuccessRate() << "%)";
		}

	private:
		detail::SuccessRateRecorder iterate_;
		detail::SuccessRateRecorder selection_;
		detail::SuccessRateRecorder simulation_;
	};
}