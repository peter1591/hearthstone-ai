#pragma once

#include "state/State.h"

namespace mcts
{
	class BoardReplayer
	{
	private:
		struct PendingAction
		{
			enum {
				kUserChoose,
				kRandom
			} type; // TODO: only for debug purpose. Can be removed in release builds
			int action;
		};

	public:
		BoardReplayer(state::State const& state)
		{
			SetBoard(state);
		}

		void SetBoard(state::State const& state)
		{
			state_ = state;
			append_actions_.clear();
		}

		void AppendUserChooseAction(int action)
		{
			append_actions_.push_back(PendingAction{ PendingAction::kUserChoose, action });
		}

		void AppendRandomAction(int action)
		{
			append_actions_.push_back(PendingAction{ PendingAction::kRandom, action });
		}

		void Replay()
		{
			state::State new_state = state_;

		}


	private:

		state::State state_;
		std::vector<PendingAction> append_actions_;
	};
}