#pragma once

#include <time.h>

#include <memory>
#include <random>
#include <sstream>

#include "state/State.h"
#include "engine/Game.h"
#include "engine/JsonSerializer.h"
#include "judge/json/Recorder.h"
#include "judge/IAgent.h"

namespace judge
{
	class NullRecorder
	{
	public:
		void Start() {}
		void RecordMainAction(state::State const& state, engine::MainOpType op) {}
		void RecordRandomAction(int exclusive_max, int action) {}
		void RecordManualAction(engine::ActionType::Types action_type, engine::ActionChoices action_choices, int action) {}
		void End(engine::Result result) {}
	};

	template <class AgentType = IAgent, class RecorderType = NullRecorder>
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
			
			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices & action_choices) final {
				if (action_type == engine::ActionType::kRandom) {
					int exclusive_max = action_choices.Size();
					int action = guide_.rand_() % exclusive_max;
					guide_.recorder_.RecordRandomAction(exclusive_max, action);
					return action;
				}
				
				int action = cb_->GetAction(action_type, action_choices, guide_.rand_);

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
		Judger(std::mt19937 & rand, RecorderType & recorder) :
			rand_(rand), action_callback_(*this),
			first_(nullptr), second_(nullptr), recorder_(recorder)
		{}

		Judger(Judger const& rhs) = delete;
		Judger & operator=(Judger const& rhs) = delete;

		void SetFirstAgent(AgentType * first) { first_ = first; }
		void SetSecondAgent(AgentType * second) { second_ = second; }

		template <class StateGetter>
		engine::Result Start(StateGetter && state_getter, std::mt19937 & random)
		{
			assert(first_);
			assert(second_);
			
			recorder_.Start();

			engine::Game game;

			game.SetStartState(state_getter(random));

			engine::Result result = engine::kResultInvalid;
			AgentType * next_agent = nullptr;
			while (true) {
				state::PlayerIdentifier next_player = game.GetCurrentState().GetCurrentPlayerId();
				if (next_player.IsFirst()) {
					next_agent = first_;
				}
				else {
					assert(next_player.IsSecond());
					next_agent = second_;
				}

				next_agent->Think(next_player,
					engine::view::BoardRefView(game.GetCurrentState(), next_player.GetSide()),
					random);

				action_callback_.Initialize(game.GetCurrentState(), next_agent);
				result = game.PerformAction(action_callback_);

				assert(result != engine::kResultInvalid);
				if (result != engine::kResultNotDetermined) break;
			}

			recorder_.End(result);

			return result;
		}

	private:
		std::mt19937 & rand_;
		ActionCallback action_callback_;
		AgentType * first_;
		AgentType * second_;
		RecorderType & recorder_;
	};
}