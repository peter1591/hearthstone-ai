#pragma once

#include <vector>
#include "MCTS/board/Board.h"
#include "MCTS/Types.h"

namespace mcts
{
	class SOMCTS;

	namespace builder
	{
		// TODO: expand this class into tree builder?
		// it's now only have two fields, and has no any flow control
		class ActionApplyState
		{
		public:
			ActionApplyState(SOMCTS & caller) :
				action_parameter_getter_(caller), random_generator_(caller),
				board_(nullptr), action_analyzer_(nullptr), is_valid_(true)
			{}

			ActionApplyState(ActionApplyState const&) = delete;
			ActionApplyState & operator=(ActionApplyState const&) = delete;

			void Start(board::Board const& board)
			{
				is_valid_ = true;
				board_ = &board;
			}

			board::Board const& GetBoard() const { return *board_; }

			Result ApplyAction(int action, FlowControl::FlowContext & flow_context, board::BoardActionAnalyzer & action_analyzer)
			{
				return board_->ApplyAction(action, action_analyzer, flow_context, random_generator_, action_parameter_getter_);
			}

			void SetValid() { is_valid_ = true; }
			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			board::ActionParameterGetter action_parameter_getter_;
			board::RandomGenerator random_generator_;

			board::Board const* board_;
			board::BoardActionAnalyzer * action_analyzer_;
			bool is_valid_;
		};
	}
}