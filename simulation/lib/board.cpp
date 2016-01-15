#include <iostream>
#include <stdexcept>

#include "board.h"

std::list<Move> Board::GetNextMoves()
{
	std::list<Move> moves;

	if (this->state.GetStage() == BoardState::STAGE_START_TURN) {
		return this->DoStartTurn();
	}

	switch (this->state.GetStage())
	{
		case BoardState::STAGE_START_TURN:
			return this->DoStartTurn();
		case BoardState::STAGE_DRAW:
			return this->DoDraw();
		default:
			throw new std::runtime_error("Unknown state");
	}
		return this->DoDraw();

	return moves;
}

std::list<Move> Board::DoStartTurn()
{
	Move move;
	std::list<Move> moves;
	BoardState next_state = this->state;
	next_state.SetStage(BoardState::STAGE_DRAW);

	move.action = Move::ACTION_STATE_TRANSFER;
	move.data.state_transfer.next_state = next_state;
	moves.push_back(move);

	return moves;
}

std::list<Move> Board::DoDraw()
{
	std::list<Move> moves;

	this->player_deck.Draw();

	// TODO

	return moves;
}

void Board::ApplyMove(const Move &move)
{
	if (move.action == Move::ACTION_STATE_TRANSFER) {
		this->state = move.data.state_transfer.next_state;
	}
}

void Board::DebugPrint() const
{
	std::cout << "=== Print Board START ===" << std::endl;

	if (this->state.IsPlayerTurn()) {
		std::cout << "Player's turn." << std::endl;
	} else {
		std::cout << "Opponent's turn." << std::endl;
	}
	std::cout << "Stage: " << this->state.GetStage() << std::endl;

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

void Move::DebugPrint() const
{
	if (this->action == ACTION_STATE_TRANSFER) {
		BoardState state = this->data.state_transfer.next_state;
		std::cout << "[STATE_TRANSFER] next_state = " << state.GetStage() << std::endl;
	} else {
		std::cout << "[UNKNOWN STATE!]" << std::endl;
	}
}
