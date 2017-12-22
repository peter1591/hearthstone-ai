#pragma once

#include <time.h>

#include <memory>
#include <random>
#include <sstream>

#include "state/State.h"
#include "engine/FlowControl/FlowController.h"
#include "engine/JsonSerializer.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "judge/Recorder.h"
#include "judge/IAgent.h"

namespace judge
{
	template <class AgentType = IAgent, class RecorderType = JsonRecorder>
	class Judger
	{
	private:
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

		class ActionCallback : public engine::IActionParameterGetter {
		public:
			ActionCallback(Judger & guide) : guide_(guide), cb_(nullptr), state_(nullptr) {}

			ActionCallback(ActionCallback const&) = delete;
			ActionCallback & operator=(ActionCallback const&) = delete;

			void Initialize(state::State const& state, AgentType * cb) {
				state_ = &state;
				cb_ = cb;
				engine::IActionParameterGetter::Initialize(*state_);
			}
			
			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final {
				int action = cb_->GetAction(action_type, action_choices);

				if (action_type == engine::ActionType::kMainAction) {
					auto main_op = analyzer_.GetMainActions()[action];
					guide_.recorder_.RecordMainAction(*state_, main_op);
					return action;
				}
				else {
					guide_.recorder_.RecordManualAction(action_type, action_choices, action);
				}

				return action;
			}

		private:
			Judger & guide_;
			AgentType * cb_;
			state::State const* state_;
		};

	public:
		Judger(std::mt19937 & rand) :
			rand_(rand), random_callback_(*this), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_(rand_)
		{}

		Judger(Judger const& rhs) = delete;
		Judger & operator=(Judger const& rhs) = delete;

		void SetFirstAgent(AgentType * first) { first_ = first; }
		void SetSecondAgent(AgentType * second) { second_ = second; }

		template <class StateGetter>
		void Start(StateGetter && state_getter, int seed)
		{
			assert(first_);
			assert(second_);
			
			recorder_.Start();
			std::mt19937 random(seed);
			state::State current_state = state_getter(random());

			engine::Result result = engine::kResultInvalid;
			AgentType * next_agent = nullptr;
			while (true) {
				if (current_state.GetCurrentPlayerId().IsFirst()) {
					next_agent = first_;
				}
				else {
					assert(current_state.GetCurrentPlayerId().IsSecond());
					next_agent = second_;
				}

				auto current_state_getter = [&](int rnd) -> state::State const& {
					// TODO: should randomize hidden information from the board view
					return current_state;
				};
				next_agent->Think(current_state.GetCurrentPlayerId(), current_state_getter, random);

				action_callback_.Initialize(current_state, next_agent);
				engine::FlowControl::FlowContext flow_context(random_callback_, action_callback_);
				engine::FlowControl::FlowController flow_controller(current_state, flow_context);
				result = flow_controller.PerformOperation();

				assert(result != engine::kResultInvalid);
				if (result != engine::kResultNotDetermined) break;
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