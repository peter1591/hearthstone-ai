#pragma once

#include <time.h>

#include <memory>
#include <random>
#include <sstream>

#include "state/State.h"
#include "engine/Game.h"
#include "engine/JsonSerializer.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "judge/Recorder.h"
#include "judge/IAgent.h"

namespace judge
{
	template <class AgentType = IAgent, class RecorderType = JsonRecorder>
	class Judger
	{
	private:
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
				if (action_type == engine::ActionType::kRandom) {
					int exclusive_max = action_choices.Size();
					int action = guide_.rand_() % exclusive_max;
					guide_.recorder_.RecordRandomAction(exclusive_max, action);
					return action;
				}
				
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
			rand_(rand), action_callback_(*this),
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

			engine::Game game;

			game.SetStartState(state_getter(random()));

			engine::Result result = engine::kResultInvalid;
			AgentType * next_agent = nullptr;
			while (true) {
				if (game.GetCurrentState().GetCurrentPlayerId().IsFirst()) {
					next_agent = first_;
				}
				else {
					assert(game.GetCurrentState().GetCurrentPlayerId().IsSecond());
					next_agent = second_;
				}

				auto current_state_getter = [&](int rnd) -> state::State const& {
					// TODO: should randomize hidden information from the board view
					return game.GetCurrentState();
				};
				next_agent->Think(game.GetCurrentState().GetCurrentPlayerId(), current_state_getter, random);

				action_callback_.Initialize(game.GetCurrentState(), next_agent);
				result = game.PerformAction(action_callback_);

				assert(result != engine::kResultInvalid);
				if (result != engine::kResultNotDetermined) break;
			}

			recorder_.End(result);
		}

	private:
		std::mt19937 & rand_;
		ActionCallback action_callback_;
		AgentType * first_;
		AgentType * second_;
		RecorderType recorder_;
	};
}