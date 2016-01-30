#include <iostream>
#include <stdexcept>
#include <functional>

#include "stages/player-turn-start.h"
#include "stages/player-choose-board-move.h"
#include "stages/player-put-minion.h"
#include "stages/player-attack.h"
#include "stages/player-turn-end.h"
#include "stages/opponent-turn-start.h"
#include "stages/opponent-choose-board-move.h"
#include "stages/opponent-put-minion.h"
#include "stages/opponent-attack.h"
#include "stages/opponent-turn-end.h"

#include "game-engine/random-generator.h"

#include "game-engine/board.h"

namespace GameEngine {

template <typename Chooser, typename... Params, typename Return = typename Chooser::ReturnType>
static Return StageFunctionCaller(Stage stage, Params & ... params)
{
	typedef StageFunctionChooser::Caller<Chooser> Caller;

#define IF_ELSE_HANDLE_STAGE_CLASS(class_name) \
	else if (stage == class_name::stage) { \
		return Caller::template Call<class_name>(params...); \
	}

	if (false) {
	}
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerChooseBoardMove)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentChooseBoardMove)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerAttack)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentAttack)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerPutMinion)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentPutMinion)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerTurnStart)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentTurnStart)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerTurnEnd)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentTurnEnd)

#undef IF_ELSE_HANDLE_STAGE_CLASS

	throw std::runtime_error("Unhandled state for StageFunctionCaller()");
}

void Board::GetNextMoves(std::vector<Move> &next_moves) const
{
	switch (this->GetStageType()) {
		case STAGE_TYPE_GAME_FLOW:
			throw std::runtime_error("This is a game flow stage, you should skip the GetNextMoves() call, and apply the Move::GetGameFlowMove() directly.");
		case STAGE_TYPE_GAME_END:
			return;

		case STAGE_TYPE_PLAYER:
		case STAGE_TYPE_OPPONENT:
			return StageFunctionCaller<StageFunctionChooser::Chooser_GetNextMoves>(this->stage, *this, next_moves);

		default:
			throw std::runtime_error("Unhandled case in GetNextMoves()");
	}
}

void Board::GetGoodMove(Move & next_move, unsigned int rand) const
{
	switch (this->GetStageType()) {
	case STAGE_TYPE_GAME_FLOW:
		throw std::runtime_error("This is a game flow stage, you should skip the GetNextMoves() call, and apply the Move::GetGameFlowMove() directly.");
	case STAGE_TYPE_GAME_END:
		return;

	case STAGE_TYPE_PLAYER:
	case STAGE_TYPE_OPPONENT:
		return StageFunctionCaller<StageFunctionChooser::Chooser_GetGoodMove>(this->stage, *this, next_move, rand);

	default:
		throw std::runtime_error("Unhandled case in GetGoodMove()");
	}
}

void Board::ApplyMove(const Move &move)
{
	if (this->GetStageType() == STAGE_TYPE_GAME_FLOW) {
		this->random_generator.SetRandomSeed(move.data.game_flow_data.rand_seed);
	}

#ifdef DEBUG
	StageType stage_type_prev = this->GetStageType();
#endif

	StageFunctionCaller<StageFunctionChooser::Chooser_ApplyMove>(this->stage, *this, move);
}

void Board::SetStateToPlayerTurnStart()
{
	this->stage = StagePlayerTurnStart::stage;
}

void Board::SetStateToPlayerChooseBoardMove()
{
	this->stage = StagePlayerChooseBoardMove::stage;
	//this->stage = StageOpponentChooseBoardMove::stage;
}

void Board::DebugPrint() const
{
	Stage stage = this->GetStage();
	StageType stage_type = this->GetStageType();

	std::cout << "=== Print Board START ===" << std::endl;

	std::cout << "HASH: " << std::hash<GameEngine::Board>()(*this) << std::endl;

	switch (stage_type) {
		case STAGE_TYPE_PLAYER:
			std::cout << "Player's turn." << std::endl;
			break;
		case STAGE_TYPE_OPPONENT:
			std::cout << "Opponent's turn." << std::endl;
			break;
		case STAGE_TYPE_GAME_FLOW:
			std::cout << "Game flow (random) turn." << std::endl;
			break;
		default:
			throw std::runtime_error("unhandled stage type");
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
		std::cout << card.GetDebugString() << " ";
	}
	std::cout << std::endl;

	std::cout << "Player hand: " << std::endl;
	std::cout << "\t";
	for (const auto &card : this->player_hand.GetCards()) {
		std::cout << card.GetDebugString() << " ";
	}
	std::cout << std::endl;

	std::cout << "Opponent minions: " << std::endl;
	this->opponent_minions.DebugPrint();

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();

	std::cout << "=== Print Board END   ===" << std::endl;
}

std::string Board::GetStageName() const
{
	return StageFunctionCaller<StageFunctionChooser::Chooser_GetStageStringName>(stage);
}

} // namespace GameEngine
