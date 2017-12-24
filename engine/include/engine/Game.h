#pragma once

#include "state/State.h"
#include "engine/FlowControl/FlowController.h"
#include "engine/FlowControl/FlowContext.h"
#include "engine/IActionParameterGetter.h"
#include "engine/Result.h"

namespace engine
{
	class Game
	{
	private:
		class RandomGenerator : public engine::FlowControl::IRandomGenerator
		{
		public:
			RandomGenerator(engine::IActionParameterGetter & callback) : callback_(callback) {}

			int Get(int exclusive_max) final {
				return callback_.GetNumber(
					engine::ActionType::kRandom,
					engine::ActionChoices(exclusive_max));
			}

		private:
			engine::IActionParameterGetter & callback_;
		};

	public:
		void SetStartState(state::State const& state) {
			state_ = state;
		}

		state::State const& GetCurrentState() const { return state_; }

		Result PerformAction(engine::IActionParameterGetter & action_cb) {
			RandomGenerator random_cb(action_cb);
			FlowControl::FlowContext flow_context(random_cb, action_cb);
			FlowControl::FlowController flow_controller(state_, flow_context);
			return flow_controller.PerformAction();
		}

	public:
		void RefCopyFrom(Game const& rhs) {
			state_.RefCopy(rhs.state_);
		}

	private:
		state::State state_;
	};
}