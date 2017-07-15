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
				board_(nullptr), is_valid_(true),
				has_saved_board_(false), saved_board_()
			{}

			ActionApplyState(ActionApplyState const&) = delete;
			ActionApplyState & operator=(ActionApplyState const&) = delete;

			void Start(board::Board & board)
			{
				is_valid_ = true;
				board_ = &board;
				has_saved_board_ = false;
			}

			board::Board const& GetBoard() const { return *board_; }

			Result ApplyAction(int action, board::BoardActionAnalyzer & action_analyzer)
			{
				return board_->ApplyAction(action, action_analyzer, random_generator_, action_parameter_getter_);
			}

			void SaveBoard() {
				assert(board_);
				saved_board_ = board_->GetState();
				has_saved_board_ = true;
			}

			void RestoreBoard() {
				assert(board_);
				assert(has_saved_board_);
				board_->SetState(saved_board_);
			}

			bool HasSavedBoard() const { return has_saved_board_; }

			void SetValid() { is_valid_ = true; }
			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			board::ActionParameterGetter action_parameter_getter_;
			board::RandomGenerator random_generator_;

			board::Board * board_;
			bool is_valid_;

			bool has_saved_board_;
			state::State saved_board_;
		};
	}
}