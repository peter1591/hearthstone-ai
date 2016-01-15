#include <iostream>

#include "board.h"

void Board::PrintBoard() const
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
