#pragma once

#include <atomic>
#include <sstream>

#include "MCTS/Config.h"

namespace mcts
{
	// Thread safety: Yes
	template <bool enabled = mcts::StaticConfigs::enable_statistic>
	class Statistic {
	public:
		void ApplyActionSucceeded(bool is_simulation) {}
		void GetDebugMessage() {}
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

		std::string GetDebugMessage() const {
			std::stringstream ss;

			ss << "Apply selection action success rate: ";
			PrintRate(ss, selection_);
			ss << std::endl;

			ss << "Apply simulation action success rate: ";
			PrintRate(ss, simulation_);
			ss << std::endl;

			ss << "Iterate success rate: ";
			PrintRate(ss, iterate_);
			ss << std::endl;

			return ss.str();
		}

	private:
		void PrintRate(std::stringstream & ss, detail::SuccessRateRecorder const& rate) const {
			ss << rate.GetSuccessCount()
				<< " / " << rate.GetTotalCount()
				<< " (" << 100.0 * rate.GetSuccessRate() << "%)";
		}

	private:
		detail::SuccessRateRecorder iterate_;
		detail::SuccessRateRecorder selection_;
		detail::SuccessRateRecorder simulation_;
	};
}