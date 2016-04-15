#pragma once

#include <list>
#include "json-parser.h"
#include "board-with-moves.h"
#include "board-initializer.h"
#include "game-engine/move.h"

// hold a starting board which comes from a certain board with certain moves
class BoardWithMoves : public BoardInitializer
{
public:
	BoardWithMoves(int start_board_rand, std::unique_ptr<BoardInitializer> board_initializer, std::list<GameEngine::Move> && pending_moves)
		: start_board_rand(start_board_rand), board_initializer(std::move(board_initializer)), pending_moves(std::move(pending_moves))
	{
	}

	void InitializeBoard(int rand_seed, GameEngine::Board & board) const
	{
		board_initializer->InitializeBoard(this->start_board_rand, board);
		for (auto const& pending_move : this->pending_moves) {
			board.ApplyMove(pending_move);
		}

		board.SetRandomSeed(rand_seed);

		std::mt19937 random_generator(rand_seed);
		this->ShuffleHiddenInformation(random_generator, board);
	}

private:
	void ShuffleHiddenInformation(std::mt19937 & random_generator, GameEngine::Board & board) const
	{
		// boards only differs with hidden information are considered equal when comparing boards
		// here we need to shuffle those information

		std::function<int()> random_number_getter = [&random_generator] {
			return random_generator();
		};

		// shuffle hidden cards
		// Note: only need to shuffle opponents hand cards
		//       the player's random cards already checked in game-flow moves
		board.opponent.hand.ShuffleHiddenInformation(random_number_getter);

		// TODO: shuffle hidden secrets
	}

private:
	int start_board_rand;
	std::unique_ptr<BoardInitializer> board_initializer;
	std::list<GameEngine::Move> pending_moves;
};