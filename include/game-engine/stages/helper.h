#ifndef STAGES_HELPER_H
#define STAGES_HELPER_H

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/board-objects/minion.h"

namespace GameEngine {

class StageHelper
{
public:
	typedef Move::PlayMinionData PlayMinionData;

public: // return true if game state changed (e.g., win/loss)
	static bool PlayerDrawCard(Player & player);

	static void GetBoardMoves(Player const& player, NextMoveGetter & next_moves, bool & all_cards_determined);
	static void GetBoardMoves_PlayMinion(Player const& player, Hand::Locator hand_card, Card const& playing_card, NextMoveGetter &next_move_getter);
	static void GetBoardMoves_EquipWeapon(Player const& player, Hand::Locator hand_card, Card const& playing_card, NextMoveGetter &next_move_getter);
	static void GetBoardMoves_PlaySpell(Player const& player, Hand::Locator hand_card, Card const& playing_card, NextMoveGetter &next_move_getter);
	static void GetBoardMoves_Attack(Player const& player, NextMoveGetter &next_move_getter);
	static void GetGoodBoardMove(unsigned int rand, Player const & player, Move &good_move);

	static bool CheckHeroMinionDead(Board & board);

	// include battle cry, and summoning minion
	static bool PlayMinion(Player & player, Card const& card, PlayMinionData const& data);

	// no battle cry
	static bool SummonMinion(Card const& card, MinionIterator & it);

	static bool EquipWeapon(Player & player, Card const& card, SlotIndex target);

	static bool PlaySpell(Player & player, Card const& card, SlotIndex target);

public:
	// handle minion/hero attack, calculate damages
	static void HandleAttack(GameEngine::Board & board, GameEngine::SlotIndex attacker_idx, GameEngine::SlotIndex attacked_idx);

	static void DealDamage(GameEngine::Board & board, SlotIndex taker_idx, int damage, bool poisonous);
	static void DealDamage(GameEngine::BoardObject taker, int damage, bool poisonous);
	static void DealDamage(GameEngine::Hero & taker, int damage, bool poisonous);
	static void DealDamage(GameEngine::Minion & taker, int damage, bool poisonous);

private:
	static SlotIndex GetTargetForForgetfulAttack(GameEngine::Board & board, SlotIndex origin_attacked);

	static void RemoveMinionsIfDead(Board & board, SlotIndex side);

	static void Fatigue(GameEngine::Board & board, SlotIndex side);
};

} // namespace GameEngine
#endif
