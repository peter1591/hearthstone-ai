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

	private:
		JsonBoardFinder(BoardInitializer * start_board, Json::Value const& json_board);

		bool Iterate();

		GameEngine::Move GetOneRandomNextMove(GameEngine::Board const& board);

	private:
		std::unique_ptr<BoardInitializer> FindBoard();

	private:
		BoardInitializer const* start_board;
		Json::Value const& json_board;

		int next_rand_seed;
		int found_start_board_rand;
		std::list<GameEngine::Move> found_applied_moves;
	};
} // namespace JsonBaordFinder