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
		JsonBoardFinder(std::unique_ptr<BoardInitializer> start_board, Json::Value const& json_board);
		std::unique_ptr<BoardInitializer> FindBoard();

	private:
		bool Iterate();
		GameEngine::Move GetOneRandomNextMove(GameEngine::Board const& board);

	private:
		std::unique_ptr<BoardInitializer> start_board;
		Json::Value const& json_board;

		std::mt19937 random_generator;
		int found_start_board_rand;
		std::list<GameEngine::Move> found_applied_moves;
	};
} // namespace JsonBaordFinder