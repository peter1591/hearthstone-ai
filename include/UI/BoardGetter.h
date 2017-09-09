#pragma once

#include <sstream>
#include <string>

#include <json/json.h>

#include "MCTS/TestStateBuilder.h"

namespace ui
{
	class BoardGetter
	{
	public:
		BoardGetter() : board_() {}

		int ResetBoard()
		{
			need_restart_ai_ = true;
			return 0;
		}

		int UpdateBoard(std::string const& board)
		{
			Json::Reader reader;
			std::stringstream ss(board);
			reader.parse(ss, board_);

			// TODO: reuse MCTS tree

			return 0;
		}

		int PrepareToRun(ui::AIController * controller, bool * restart_ai)
		{
			if (need_restart_ai_) {
				*restart_ai = true;
				return 0;
			}

			// TODO: reuse last MCTS tree
			(void)controller;
			*restart_ai = true;
			return 0;
		}

		state::State GetStartBoard(int seed)
		{
			// TODO: use board
			return TestStateBuilder().GetState(seed);
		}

	private:
		bool need_restart_ai_;
		Json::Value board_;
	};
}