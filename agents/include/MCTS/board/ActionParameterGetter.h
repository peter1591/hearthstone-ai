#pragma once

#include <vector>

#include "state/State.h"
#include "MCTS/Types.h"
#include "MCTS/board/ActionChoices.h"
#include "judge/IActionParameterGetter.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class ActionParameterGetter : public judge::IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : callback_(callback) {}

			void SetMainOpIndex(int main_op_idx) { main_op_idx_ = main_op_idx; }

			int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) final;

		private:
			SOMCTS & callback_;
			int main_op_idx_;
		};
	}
}