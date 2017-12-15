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
	template <class AgentType = IAgent, class RecorderType = JsonRecorder>
	class Judger
	{
	public:
		using StartingStateGetter = std::function<state::State()>;

		class RandomCallback : public state::IRandomGenerator {
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

		class ActionCallback : public judge::IActionParameterGetter {
		public:
			ActionCallback(Judger & guide) : guide_(guide), cb_(nullptr), state_(nullptr) {}

			ActionCallback(ActionCallback const&) = delete;
			ActionCallback & operator=(ActionCallback const&) = delete;

			void SetCallback(AgentType * cb) { cb_ = cb; }
			void SetState(state::State const& state) { state_ = &state; }
			
			FlowControl::utils::MainOpType ChooseMainOp() {
				auto main_op = cb_->GetMainAction();
				guide_.recorder_.RecordMainAction(*state_, main_op);
				return main_op;
			}

			int GetNumber(mcts::ActionType::Types action_type, mcts::board::ActionChoices const& action_choices) final {
				int action = cb_->GetSubAction(action_type, action_choices);
				guide_.recorder_.RecordManualAction(action_type, action_choices, action);
				return action;
			}

		private:
			Judger & guide_;
			AgentType * cb_;
			state::State const* state_;
		};

		Judger(std::mt19937 & rand) :
			rand_(rand), random_callback_(*this), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_(rand_)
		{}

		Judger(Judger const& rhs) = delete;
		Judger & operator=(Judger const& rhs) = delete;

		void SetFirstAgent(AgentType * first) { first_ = first; }
		void SetSecondAgent(AgentType * second) { second_ = second; }

		template <class ProgressCallback, class IterationProgressCallback>
		void Start(StartingStateGetter state_getter, int seed,
			ProgressCallback && cb, IterationProgressCallback && iteration_cb)
		{
			state::State current_state = state_getter();
			assert(first_);
			assert(second_);
			
			recorder_.Start();
			std::mt19937 random(seed);

			FlowControl::Result result = FlowControl::kResultInvalid;
			AgentType * next_agent = nullptr;
			action_callback_.SetState(current_state);
			while (true) {
				cb(current_state);

				if (current_state.GetCurrentPlayerId().IsFirst()) {
					next_agent = first_;
				}
				else {
					assert(current_state.GetCurrentPlayerId().IsSecond());
					next_agent = second_;
				}

				next_agent->Think(current_state, random, iteration_cb);

				std::vector<size_t> playable_cards;
				FlowControl::ValidActionGetter(current_state).ForEachPlayableCard([&](size_t idx) {
					playable_cards.push_back(idx);
					return true;
				});
				std::vector<int> attackers;
				FlowControl::ValidActionGetter(current_state).ForEachAttacker([&](int idx) {
					attackers.push_back(idx);
					return true;
				});
				FlowControl::utils::ActionApplier action_applier(attackers, playable_cards);

				action_callback_.SetCallback(next_agent);
				result = action_applier.Apply(current_state, action_callback_, random_callback_);

				assert(result != FlowControl::kResultInvalid);
				if (result != FlowControl::kResultNotDetermined) break;
			}

			recorder_.End(result);
		}

	private:
		std::mt19937 & rand_;
		RandomCallback random_callback_;
		ActionCallback action_callback_;
		AgentType * first_;
		AgentType * second_;
		RecorderType recorder_;
	};
}