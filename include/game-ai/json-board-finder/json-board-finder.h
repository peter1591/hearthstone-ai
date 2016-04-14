#pragma once

#include <memory>
#include "json/json.h"
#include "game-ai/board-initializer/board-with-moves.h"
#include "game-ai/board-initializer/board-initializer.h"
#include "game-ai/mcts.h"

namespace JsonBoardFinder
{
	class JsonBoardFinder
	{
	public:
		static void UpdateMCTS(MCTS & mcts, Json::Value const& json_board);
		static std::unique_ptr<BoardInitializer> FindBoard(BoardInitializer * start_board, Json::Value const& json_board);
	};
} // namespace JsonBaordFinder