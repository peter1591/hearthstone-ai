#include <iostream>
#include <stdexcept>

#include "board.h"
#include "stages.h"

void Board::GetNextMoves(std::vector<Move> &next_moves) const
{
	switch (this->stage)
	{
		case STAGE_PLAYER_TURN_START:
			return StagePlayerTurnStart::GetNextMoves(*this, next_moves);

		case STAGE_PLAYER_TURN_END:
			return StageEndTurn::GetNextMoves(*this, next_moves);

		case STAGE_PLAYER_CHOOSE_BOARD_MOVE:
			return StageChooseBoardMove::GetNextMoves(*this, next_moves);

		case STAGE_WIN:
		case STAGE_LOSS:
			return;

		case STAGE_UNKNOWN:
			throw std::runtime_error("Unknown state for GetNextMoves()");
	}
	throw std::runtime_error("Unhandled state for GetNextMoves()");
}

void Board::ApplyMove(const Move &move)
{
	switch (this->stage)
	{
		case STAGE_PLAYER_TURN_START:
			return StagePlayerTurnStart::ApplyMove(*this, move);
		case STAGE_PLAYER_TURN_END:
			return StageEndTurn::ApplyMove(*this, move);
		case STAGE_PLAYER_CHOOSE_BOARD_MOVE:
			return StageChooseBoardMove::ApplyMove(*this, move);

		case STAGE_WIN:
		case STAGE_LOSS:
			throw std::runtime_error("ApplyMove() should not be called when it's a win/loss");

		case STAGE_UNKNOWN:
			throw std::runtime_error("Unknown state for ApplyMove()");
	}
	throw std::runtime_error("Unhandled state for ApplyMove()");
}

void Board::GetStage(bool &is_player_turn, bool &is_random_node, Board::Stage stage ) const
{
	// TODO
}

void Board::DebugPrint() const
{
	bool is_player_turn, is_random_node;
	Stage stage;

	this->GetStage(is_player_turn, is_random_node, stage);

	std::cout << "=== Print Board START ===" << std::endl;

	if (is_player_turn) {
		std::cout << "Player's turn." << std::endl;
	} else {
		std::cout << "Opponent's turn." << std::endl;
	}
	std::cout << "Stage: " << stage << std::endl;

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

		case Move::ACTION_PLAY_HAND_CARD_MINION:
			std::cout << "[Play hand card minion] hand idx = " << this->data.play_hand_card_minion_data.idx_hand_card
				<< ", put location = " << this->data.play_hand_card_minion_data.location;
			break;
		case Move::ACTION_ATTACK:
			std::cout << "[Attack] attacking = " << this->data.attack_data.attacking_idx
				<< ", attacked = " << this->data.attack_data.attacked_idx;
			break;
		case Move::ACTION_END_TURN:
			std::cout << "[End turn]";
			break;

		default:
			throw std::runtime_error("unknown action for Move::DebugPrint()");
			break;
	}
}
