#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>

#include "game-engine/stages/player-turn-start.h"
#include "game-engine/stages/player-choose-board-move.h"
#include "game-engine/stages/player-put-minion.h"
#include "game-engine/stages/player-equip-weapon.h"
#include "game-engine/stages/player-play-spell.h"
#include "game-engine/stages/player-attack.h"
#include "game-engine/stages/player-use-hero-power.h"
#include "game-engine/stages/player-turn-end.h"
#include "game-engine/stages/opponent-turn-start.h"
#include "game-engine/stages/opponent-choose-board-move.h"
#include "game-engine/stages/opponent-put-minion.h"
#include "game-engine/stages/opponent-equip-weapon.h"
#include "game-engine/stages/opponent-play-spell.h"
#include "game-engine/stages/opponent-attack.h"
#include "game-engine/stages/opponent-use-hero-power.h"
#include "game-engine/stages/opponent-turn-end.h"

#include "game-engine/random/random-generator.h"

#include "game-engine/board.h"

namespace GameEngine {

template <typename Chooser, typename... Params>
inline typename Chooser::ReturnType Board::StageFunctionCaller(Stage const stage, Params&&... params)
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
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerEquipWeapon)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentEquipWeapon)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerPlaySpell)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentPlaySpell)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerUseHeroPower)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentUseHeroPower)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerTurnStart)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentTurnStart)
	IF_ELSE_HANDLE_STAGE_CLASS(StagePlayerTurnEnd)
	IF_ELSE_HANDLE_STAGE_CLASS(StageOpponentTurnEnd)

#undef IF_ELSE_HANDLE_STAGE_CLASS

	throw std::runtime_error("Unhandled state for StageFunctionCaller()");
}

inline void Board::GetNextMoves(GameEngine::Move & next_move, bool & is_deterministic) const
{
	switch (this->GetStageType()) {
	case STAGE_TYPE_GAME_FLOW:
		next_move.action = GameEngine::Move::ACTION_GAME_FLOW;
		is_deterministic = false; // although the next move is always the 'game-flow', but it may be non-deterministic when applying that
		return;

	case STAGE_TYPE_GAME_END:
		throw std::runtime_error("Game end stage has no next move");

	case STAGE_TYPE_PLAYER:
	case STAGE_TYPE_OPPONENT:
		throw std::runtime_error("This is not a game flow stage, you should call GetNextMoves() and get the next-move-getter");

	default:
		throw std::runtime_error("Unhandled case in GetNextMoves()");
	}
}

inline void Board::GetNextMoves(NextMoveGetter &next_move_getter, bool & is_deterministic) const
{
	switch (this->GetStageType()) {
		case STAGE_TYPE_GAME_FLOW:
			throw std::runtime_error("This is a game flow stage, you should call the GetNextMove() to get the next move directly.");

		case STAGE_TYPE_GAME_END:
			throw std::runtime_error("Game end stage has no next move");

		case STAGE_TYPE_PLAYER:
		case STAGE_TYPE_OPPONENT:
			return Board::StageFunctionCaller<StageFunctionChooser::Chooser_GetNextMoves>(this->stage, *this, next_move_getter, is_deterministic);

		default:
			throw std::runtime_error("Unhandled case in GetNextMoves()");
	}
}

inline void Board::GetGoodMove(Move & next_move, unsigned int rand) const
{
	switch (this->GetStageType()) {
	case STAGE_TYPE_GAME_FLOW:
		throw std::runtime_error("You cannot choose a good move when in a game-flow stage.");

	case STAGE_TYPE_GAME_END:
		throw std::runtime_error("You cannot choose a good move when in a game-end stage.");

	case STAGE_TYPE_PLAYER:
	case STAGE_TYPE_OPPONENT:
		return StageFunctionCaller<StageFunctionChooser::Chooser_GetGoodMove>(this->stage, *this, next_move, rand);

	default:
		throw std::runtime_error("Unhandled case in GetGoodMove()");
	}
}

inline void Board::ApplyMove(const Move &move, bool * introduced_random)
{
	this->random_generator.SetRandomSeed(this->random_seed_manager.GetNextRandomSeed());

	if (introduced_random != nullptr) this->random_generator.ClearFlag_HasCalled();
	StageFunctionCaller<StageFunctionChooser::Chooser_ApplyMove>(this->stage, *this, move);
	if (introduced_random != nullptr) *introduced_random = this->random_generator.GetFlag_HasCalled();
}

inline Board::Board() :
	player(*this, this->opponent, SlotIndex::SLOT_PLAYER_SIDE),
	opponent(*this, this->player, SlotIndex::SLOT_OPPONENT_SIDE),
	hook_manager(*this),
	object_manager(*this),
	stage(STAGE_UNKNOWN)
{
}

inline Board::~Board()
{
	this->player.Destroy();
	this->opponent.Destroy();
}

inline Board::Board(const Board & rhs) :
	player(*this, this->opponent, rhs.player),
	opponent(*this, this->player, rhs.opponent),
	hook_manager(*this),
	object_manager(*this)
{
	this->stage = rhs.stage;
	this->random_generator = rhs.random_generator;
	this->data = rhs.data;

	this->random_seed_manager = rhs.random_seed_manager;
}

inline Board Board::Clone(Board const & rhs)
{
	return Board(rhs);
}

inline Board::Board(Board && rhs) :
	player(*this, this->opponent, std::move(rhs.player)),
	opponent(*this, this->player, std::move(rhs.opponent)),
	hook_manager(*this),
	object_manager(*this),
	stage(std::move(rhs.stage)),
	random_generator(std::move(rhs.random_generator)),
	data(std::move(rhs.data)),
	random_seed_manager(std::move(rhs.random_seed_manager))
{
}

inline Board & Board::operator=(Board && rhs)
{
	if (this != &rhs) {
		this->player = std::move(rhs.player);
		this->opponent = std::move(rhs.opponent);

		this->stage = std::move(rhs.stage);
		this->random_generator = std::move(rhs.random_generator);
		this->data = std::move(rhs.data);

		this->random_seed_manager = std::move(rhs.random_seed_manager);
	}

	return *this;
}

inline void Board::SetRandomSeed(unsigned int random_seed)
{
	this->random_seed_manager.Initialize(random_seed);
	this->random_generator.SetRandomSeed(this->random_seed_manager.GetNextRandomSeed());
}

inline void Board::SetStateToPlayerChooseBoardMove()
{
	this->stage = StagePlayerChooseBoardMove::stage;
}

inline void Board::SetStateToOpponentChooseBoardMove()
{
	this->stage = StageOpponentChooseBoardMove::stage;
}

inline void Board::DebugPrint() const
{
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

	std::cout << "Stage: [" << this->stage << "] "
	   << StageFunctionCaller<StageFunctionChooser::Chooser_GetStageStringName>(this->stage)
	   << std::endl;

	std::cout << "Opponent stat: " << this->opponent.stat.GetDebugString() << std::endl;
	std::cout << "Opponent cards: " << this->opponent.hand.GetDebugString() << std::endl;

	std::cout << "Player stat: " << this->player.stat.GetDebugString() << std::endl;
	std::cout << "Player cards: " << this->player.hand.GetDebugString() << std::endl;

	std::cout << "Player Hero: " << this->player.hero.GetDebugString() << std::endl;
	std::cout << "Opponent Hero: " << this->opponent.hero.GetDebugString() << std::endl;

	std::cout << "Opponent minions: " << std::endl;
	this->opponent.minions.DebugPrint();

	std::cout << "Player minions: " << std::endl;
	this->player.minions.DebugPrint();

	std::cout << "Player total spell damage: " << this->player.GetTotalSpellDamage() << std::endl;
	std::cout << "Opponent total spell damage: " << this->opponent.GetTotalSpellDamage() << std::endl;

	std::cout << "=== Print Board END   ===" << std::endl;
}

inline std::string Board::GetStageName() const
{
	return StageFunctionCaller<StageFunctionChooser::Chooser_GetStageStringName>(stage);
}

inline bool Board::operator==(const Board &rhs) const
{
	if (this->stage != rhs.stage) return false;

	if (this->player != rhs.player) return false;
	if (this->opponent != rhs.opponent) return false;

	switch (this->stage) {
	case STAGE_WIN:
	case STAGE_LOSS:
		break;

	case STAGE_PLAYER_PUT_MINION:
	case STAGE_OPPONENT_PUT_MINION:
		if (this->data.play_hand_minion_data != rhs.data.play_hand_minion_data) return false;
		break;

	case STAGE_PLAYER_EQUIP_WEAPON:
	case STAGE_OPPONENT_EQUIP_WEAPON:
	case STAGE_PLAYER_PLAY_SPELL:
	case STAGE_OPPONENT_PLAY_SPELL:
		if (this->data.play_hand_card_data != rhs.data.play_hand_card_data) return false;
		break;

	case STAGE_PLAYER_USE_HERO_POWER:
	case STAGE_OPPONENT_USE_HERO_POWER:
		if (this->data.use_hero_power_data != rhs.data.use_hero_power_data) return false;
		break;

	case STAGE_PLAYER_ATTACK:
	case STAGE_OPPONENT_ATTACK:
		if (this->data.attack_data != rhs.data.attack_data) return false;
		break;

	case STAGE_PLAYER_CHOOSE_BOARD_MOVE:
	case STAGE_OPPONENT_CHOOSE_BOARD_MOVE:
	case STAGE_PLAYER_TURN_START:
	case STAGE_PLAYER_TURN_END:
	case STAGE_OPPONENT_TURN_START:
	case STAGE_OPPONENT_TURN_END:
		break;

	default:
		throw std::runtime_error("unknown stage");
		break;
	}

	return true;
}

inline bool Board::operator!=(Board const& rhs) const
{
	return !(*this == rhs);
}

} // namespace GameEngine
