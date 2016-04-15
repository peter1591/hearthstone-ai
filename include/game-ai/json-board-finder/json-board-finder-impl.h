#pragma once

#include "json-board-finder.h"
#include "board-comparator.h"

namespace JsonBoardFinder
{
	inline void JsonBoardFinder::UpdateMCTS(MCTS & mcts, Json::Value const & json_board)
	{
		JsonBoardFinder finder(mcts.GetBoardInitializer(), json_board);

		auto new_board_initializer = finder.FindBoard();

		std::mt19937 random_generator((unsigned int)time(nullptr));
		mcts.Initialize(random_generator(), std::move(new_board_initializer));
	}

	inline JsonBoardFinder::JsonBoardFinder(BoardInitializer * start_board, Json::Value const& json_board)
		: start_board(start_board), json_board(json_board), random_generator() // use a better random seed?
	{
	}

	inline bool JsonBoardFinder::Iterate()
	{
		this->found_applied_moves.clear();
		this->found_start_board_rand = this->random_generator();

		GameEngine::Board board;
		this->start_board->InitializeBoard(this->found_start_board_rand, board);

		while (true) {
			GameEngine::Move move = this->GetOneRandomNextMove(board);
			board.ApplyMove(move);
			this->found_applied_moves.push_back(move);

			switch (board.GetStageType()) {
			case GameEngine::STAGE_TYPE_PLAYER:
			case GameEngine::STAGE_TYPE_OPPONENT:
				// reaches the next move -> search ends
				return BoardComparator::IsEqual(board, this->json_board);

			case GameEngine::STAGE_TYPE_GAME_END:
				return false;

			default:
				break;
			}
		}
	}

	inline GameEngine::Move JsonBoardFinder::GetOneRandomNextMove(GameEngine::Board const & board)
	{
		if (board.GetStageType() == GameEngine::STAGE_TYPE_GAME_FLOW) {
			GameEngine::Move move;
			bool is_deterministic;
			board.GetNextMoves(move, is_deterministic);
			return std::move(move);
		}
		else {
			GameEngine::NextMoveGetter next_move_getter;
			bool is_deterministic;
			board.GetNextMoves(next_move_getter, is_deterministic);

			std::vector<GameEngine::Move> next_moves;
			while (true) {
				GameEngine::Move move;
				if (next_move_getter.GetNextMove(board, move) == false) break;
				next_moves.push_back(move);
			}

			int count = next_moves.size();
			if (count == 0) throw std::runtime_error("no next move is available");
			int r = this->random_generator() % count;
			return next_moves[r];
		}

		return GameEngine::Move();
	}

	inline std::unique_ptr<BoardInitializer> JsonBoardFinder::FindBoard()
	{
		std::cerr << "FindBoard() called with new json board" << std::endl;

		int iteration_count = 0;
		while (!this->Iterate())
		{
			++iteration_count;
		}

		std::cerr << "FindBoard() found a board from start rand " << this->found_start_board_rand << "  via following moves:" << std::endl;
		for (auto const& move : this->found_applied_moves) {
			std::cerr << "@@@@ " << move.GetDebugString() << std::endl;
		}

		return std::make_unique<BoardWithMoves>(this->found_start_board_rand, std::move(this->start_board), std::move(this->found_applied_moves));
	}

} // namespace JsonBoardFinder