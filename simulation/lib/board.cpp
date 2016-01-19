#include <iostream>
#include <stdexcept>

#include "stages/player-turn-start.h"
#include "stages/player-choose-board-move.h"
#include "stages/player-turn-end.h"
#include "stages/opponent-turn-start.h"
#include "stages/opponent-choose-board-move.h"
#include "stages/opponent-turn-end.h"

#include "board.h"

template <typename Chooser, typename... Params, typename Return = typename Chooser::ReturnType>
static Return StageFunctionCaller(Stage stage, Params & ... params)
{
	typedef StageFunctionChooser::Caller<Chooser> Caller;

#define SWITCH_CASE_HANDLE_CLASS(class_name) \
	case class_name::stage: \
		return Caller::template Call<class_name>(params...);

	switch (stage)
	{
		SWITCH_CASE_HANDLE_CLASS(StagePlayerTurnStart);
		SWITCH_CASE_HANDLE_CLASS(StagePlayerChooseBoardMove);
		SWITCH_CASE_HANDLE_CLASS(StagePlayerTurnEnd);
		SWITCH_CASE_HANDLE_CLASS(StageOpponentTurnStart);
		SWITCH_CASE_HANDLE_CLASS(StageOpponentChooseBoardMove);
		SWITCH_CASE_HANDLE_CLASS(StageOpponentTurnEnd);

		case STAGE_WIN:
		case STAGE_LOSS:
		case STAGE_UNKNOWN:
			break;
	}

#undef SWITCH_CASE_HANDLE_CLASS

	throw std::runtime_error("Unhandled state for StageFunctionCaller()");
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

void Board::GetStage(Stage &stage, StageType &type) const
{
	type = StageFunctionCaller<StageFunctionChooser::Chooser_GetStageType>(this->stage);
	stage = this->stage;
}

void Board::SetStateToPlayerTurnStart()
{
	this->stage = StagePlayerTurnStart::stage;
}

void Board::DebugPrint() const
{
	Stage stage;
	StageType stage_type;

	this->GetStage(stage, stage_type);

	std::cout << "=== Print Board START ===" << std::endl;

	switch (stage_type) {
		case STAGE_TYPE_PLAYER:
			std::cout << "Player's turn." << std::endl;
			break;
		case STAGE_TYPE_OPPONENT:
			std::cout << "Opponent's turn." << std::endl;
			break;
		case STAGE_TYPE_GAME_FLOW:
			std::cout << "Game flow (random) turn." << std::endl;
	}

	std::cout << "Stage: [" << stage << "] "
	   << StageFunctionCaller<StageFunctionChooser::Chooser_GetStageStringName>(stage)
	   << std::endl;

	std::cout << "Opponent stat: " << this->opponent_stat.GetDebugString() << std::endl;
	std::cout << "Opponent cards: " << this->opponent_cards.GetDebugString() << std::endl;

	std::cout << "Player stat: " << this->player_stat.GetDebugString() << std::endl;
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

	std::cout << "Opponent minions: " << std::endl;
	this->opponent_minions.DebugPrint();

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();

	std::cout << "=== Print Board END   ===" << std::endl;
}
