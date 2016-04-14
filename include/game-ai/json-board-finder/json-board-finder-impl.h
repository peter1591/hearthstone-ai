#pragma once

#include "json-board-finder.h"

namespace JsonBoardFinder
{
	inline void JsonBoardFinder::UpdateMCTS(MCTS & mcts, Json::Value const & json_board)
	{
		auto new_board_initializer = FindBoard(mcts.GetBoardInitializer(), json_board);

		std::mt19937 random_generator((unsigned int)time(nullptr));
		mcts.Initialize(random_generator(), std::move(new_board_initializer));
	}

	inline std::unique_ptr<BoardInitializer> JsonBoardFinder::FindBoard(BoardInitializer * start_board, Json::Value const& json_board)
	{
		std::cerr << "FindBoard() called with new json board" << std::endl;

		return std::make_unique<BoardJsonParser>(json_board);
	}

} // namespace JsonBoardFinder