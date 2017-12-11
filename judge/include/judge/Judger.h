#pragma once

#include <time.h>

#include <memory>
#include <random>
#include <sstream>

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/JsonSerializer.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/RandomGenerator.h"
#include "judge/Recorder.h"
#include "judge/IAgent.h"

namespace judge
{
	class Judger
	{
	public:
		using StartingStateGetter = std::function<state::State()>;

		class RandomCallback : public mcts::board::IRandomGenerator {
		public:
			RandomCallback(Judger & guide) : guide_(guide) {}
			
			int Get(int exclusive_max) final {
				int action = guide_.rand_() % exclusive_max;
				guide_.recorder_.RecordRandomAction(exclusive_max, action);
				return action;
			}

		private:
			Judger & guide_;
		};

		class ActionCallback : public mcts::board::IActionParameterGetter {
		public:
			ActionCallback(Judger & guide) : guide_(guide), cb_(nullptr), main_op_(FlowControl::utils::MainOpType::kMainOpInvalid) {}

			ActionCallback(ActionCallback const&) = delete;
			ActionCallback & operator=(ActionCallback const&) = delete;

			void SetCallback(IAgent * cb) { cb_ = cb; }
			
			void SetMainOp(FlowControl::utils::MainOpType main_op) { main_op_ = main_op; }
			FlowControl::utils::MainOpType ChooseMainOp() { return main_op_; }

			int GetNumber(mcts::ActionType::Types action_type, mcts::board::ActionChoices const& action_choices) final {
				int action = cb_->GetSubAction(action_type, action_choices);
				guide_.recorder_.RecordManualAction(action_type, action_choices, action);
				return action;
			}

		private:
			Judger & guide_;
			IAgent * cb_;
			FlowControl::utils::MainOpType main_op_;
		};

		Judger(std::mt19937 & rand) :
			rand_(rand), random_callback_(*this), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_(rand_)
		{}

		Judger(Judger const& rhs) = delete;
		Judger & operator=(Judger const& rhs) = delete;

		void SetFirstCompetitor(IAgent * first) { first_ = first; }
		void SetSecondCompetitor(IAgent * second) { second_ = second; }

		template <class ProgressCallback, class IterationProgressCallback>
		void Start(StartingStateGetter state_getter, int threads, int seed, int tree_samples,
			ProgressCallback && cb, IterationProgressCallback && iteration_cb)
		{
			state::State current_state = state_getter();
			assert(first_);
			assert(second_);
			
			recorder_.Start();

			mcts::Result result = mcts::Result::kResultInvalid;
			IAgent * next_competitor = nullptr;
			while (true) {
				cb(current_state);

				if (current_state.GetCurrentPlayerId().IsFirst()) {
					next_competitor = first_;
				}
				else {
					assert(current_state.GetCurrentPlayerId().IsSecond());
					next_competitor = second_;
				}

				next_competitor->Think(current_state, threads, seed, tree_samples, iteration_cb);

				FlowControl::FlowContext flow_context;

				int main_action = next_competitor->GetMainAction();
				auto action_analyzer = next_competitor->GetActionApplier();
				auto main_op = action_analyzer.GetMainOpType(main_action);

				recorder_.RecordMainAction(current_state, main_op);
				action_callback_.SetCallback(next_competitor);
				action_callback_.SetMainOp(main_op);

				auto flow_result = action_analyzer.GetActionApplierByRefThis().Apply(
					current_state, action_callback_, random_callback_);
				result = mcts::Result::ConvertFrom(flow_result);

				assert(result.type_ != mcts::Result::kResultInvalid);
				if (result.type_ != mcts::Result::kResultNotDetermined) {
					break;
				}
			}

			recorder_.End(result);
		}

	private:
		std::mt19937 & rand_;
		RandomCallback random_callback_;
		ActionCallback action_callback_;
		IAgent * first_;
		IAgent * second_;
		Recorder recorder_;
	};
}