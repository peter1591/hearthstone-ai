#include <iostream>
#include <stdexcept>

#include "board.h"
#include "stages.h"

template <typename Chooser, typename... Params, typename Return = typename Chooser::ReturnType>
static Return StageFunctionCaller(Board::Stage stage, Params & ... params)
{
	typedef StageFunctionChooser::Caller<Chooser> Caller;

	switch (stage)
	{
		case Board::STAGE_PLAYER_TURN_START:
			return Caller::template Call<StagePlayerTurnStart>(params...);

		case Board::STAGE_PLAYER_TURN_END:
			return Caller::template Call<StagePlayerTurnEnd>(params...);

		case Board::STAGE_PLAYER_CHOOSE_BOARD_MOVE:
			return Caller::template Call<StagePlayerChooseBoardMove>(params...);

		default:
			throw std::runtime_error("Unhandled state for StageFunctionCaller()");
	}
}

void Board::GetNextMoves(std::vector<Move> &next_moves) const
{
	switch (this->stage)
	{
		case STAGE_WIN:
		case STAGE_LOSS:
			return;

		default:
			return StageFunctionCaller<StageFunctionChooser::Chooser_GetNextMoves>(this->stage, *this, next_moves);
	}
}

void Board::ApplyMove(const Move &move)
{
	return StageFunctionCaller<StageFunctionChooser::Chooser_ApplyMove>(this->stage, *this, move);
}

void Board::GetStage(bool &is_player_turn, bool &is_random_node, Board::Stage &stage ) const
{
	is_player_turn = StageFunctionCaller<StageFunctionChooser::Chooser_IsPlayerTurn>(this->stage);
	is_random_node = StageFunctionCaller<StageFunctionChooser::Chooser_IsRandomNode>(this->stage);
	stage = this->stage;
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
