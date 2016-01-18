#include <iostream>
#include <stdexcept>

#include "board.h"
#include "stages.h"

void Board::GetNextMoves(std::vector<Move> &next_moves) const
{
	switch (this->state.GetStage())
	{
		case BoardState::STAGE_START_TURN:
			return StageStartTurn::GetNextMoves(*this, next_moves);

		case BoardState::STAGE_END_TURN:
			return StageEndTurn::GetNextMoves(*this, next_moves);

		case BoardState::STAGE_CHOOSE_BOARD_MOVE:
			return StageChooseBoardMove::GetNextMoves(*this, next_moves);

		case BoardState::STAGE_CHOOSE_PUT_MINION_LOCATION:
			return StageChoosePutMinionLocation::GetNextMoves(*this, next_moves);

		case BoardState::STAGE_WIN:
		case BoardState::STAGE_LOSS:
			return;

		case BoardState::STAGE_UNKNOWN:
			throw std::runtime_error("Unknown state for GetNextMoves()");
	}
	throw std::runtime_error("Unhandled state for GetNextMoves()");
}

void Board::ApplyMove(const Move &move)
{
	switch (this->state.GetStage())
	{
		case BoardState::STAGE_START_TURN:
			return StageStartTurn::ApplyMove(*this, move);
		case BoardState::STAGE_END_TURN:
			return StageEndTurn::ApplyMove(*this, move);
		case BoardState::STAGE_CHOOSE_BOARD_MOVE:
			return StageChooseBoardMove::ApplyMove(*this, move);
		case BoardState::STAGE_CHOOSE_PUT_MINION_LOCATION:
			return StageChoosePutMinionLocation::ApplyMove(*this, move);

		case BoardState::STAGE_WIN:
		case BoardState::STAGE_LOSS:
			throw std::runtime_error("ApplyMove() should not be called when it's a win/loss");

		case BoardState::STAGE_UNKNOWN:
			throw std::runtime_error("Unknown state for ApplyMove()");
	}
	throw std::runtime_error("Unhandled state for ApplyMove()");
}

bool BoardState::IsRandomNode() const
{
	switch (this->stage)
	{
		case STAGE_START_TURN:
			return StageStartTurn::is_random_node;
		case STAGE_END_TURN:
			return StageEndTurn::is_random_node;
		case STAGE_CHOOSE_BOARD_MOVE:
			return StageChooseBoardMove::is_random_node;
		case STAGE_CHOOSE_PUT_MINION_LOCATION:
			return StageChoosePutMinionLocation::is_random_node;

		case BoardState::STAGE_WIN:
		case BoardState::STAGE_LOSS:
			throw std::runtime_error("IsRandomNode() should not be called when it's a win/loss");

		case BoardState::STAGE_UNKNOWN:
			throw std::runtime_error("Unknown state for IsRandomNode()");
	}
	throw std::runtime_error("Unhandled state for IsRandomNode()");
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
	std::cout << "\t";
	for (const auto &card : this->player_deck.GetCards()) {
		std::cout << card.id << " ";
	}
	std::cout << std::endl;

	std::cout << "Player hand: " << std::endl;
	std::cout << "\t";
	for (const auto &card : this->player_hand.GetCards()) {
		std::cout << card.id << " ";
	}
	std::cout << std::endl;

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();

	std::cout << "=== Print Board END   ===" << std::endl;
}

void Move::DebugPrint() const
{
	switch (this->action)
	{
		case Move::ACTION_GAME_FLOW:
			std::cout << "[Game flow]";
			break;

		case Move::ACTION_PLAY_HAND_CARD:
			std::cout << "[Play hand card] idx = " << this->data.play_hand_card_data.idx;
			break;
		case Move::ACTION_ATTACK:
			std::cout << "[Attack] attacking = " << this->data.attack_data.attacking_idx
				<< ", attacked = " << this->data.attack_data.attacked_idx;
			break;
		case Move::ACTION_END_TURN:
			std::cout << "[End turn]";
			break;

		case Move::ACTION_CHOOSE_PUT_MINION_LOCATION:
			std::cout << "[Choose put minion location:] idx = " << this->data.choose_put_minion_location_data.put_location;
			break;

		default:
			throw std::runtime_error("unknown action for Move::DebugPrint()");
			break;
	}
}
