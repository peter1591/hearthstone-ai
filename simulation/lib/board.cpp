#include <iostream>

#include "board.h"

static Board::State

std::list<Move> Board::GetNextMoves()
{
	Board::State stage = (Board::State)(this->state & Board::STATE_MASK_STAGE);
	if (stage == Board::STATE_START_TURN) {
		Move move;
		move.next_state = BoardState::STATE_
	}
}

void Board::DebugPrint() const
{
	std::cout << "=== Print Board START ===" << std::endl;

	std::cout << "Player deck: " << std::endl;
	for (const auto &card : this->player_deck.GetCards()) {
		std::cout << "\t" << card.id << std::endl;
	}

	std::cout << "Player hand: " << std::endl;
	for (const auto &card : this->player_hand.GetCards()) {
		std::cout << "\t" << card.id << std::endl;
	}

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();

	std::cout << "=== Print Board END   ===" << std::endl;
}
