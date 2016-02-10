#ifndef STAGES_HELPER_H
#define STAGES_HELPER_H

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageHelper
{
public:
	typedef Move::PlayMinionData PlayMinionData;

public: // return true if game state changed (e.g., win/loss)
	static bool PlayerDrawCard(Board &board);
	static bool OpponentDrawCard(Board &board);

	static bool CheckHeroMinionDead(Board & board);

	// include battle cry, and summoning minion
	static bool PlayMinion(Board & board, Card const& card, SlotIndex playing_side, PlayMinionData const& data);

	// no battle cry
	static bool SummonMinion(Board & board, Card const& card, BoardObjects::MinionsIteratorWithIndex location);

public:
	// handle minion/hero attack, calculate damages
	static void HandleAttack(GameEngine::Board & board, GameEngine::SlotIndex attacker_idx, GameEngine::SlotIndex attacked_idx);

	static void DealDamage(GameEngine::Board & board, SlotIndex taker_idx, int damage);
	static void DealDamage(GameEngine::Board & board, BoardObjects::Hero * target, int damage);
	static void DealDamage(GameEngine::Board & board, BoardObjects::MinionsIteratorWithIndex & target, int damage);

private:
	static SlotIndex GetTargetForForgetfulAttack(GameEngine::Board & board, SlotIndex origin_attacked);

	static void RemoveMinionsIfDead(Board & board, SlotIndex side);

	static void Fatigue(GameEngine::Board & board, SlotIndex side);
};

inline bool StageHelper::PlayerDrawCard(Board & board)
{
	if (board.player_deck.GetCards().empty()) {
		StageHelper::Fatigue(board, SLOT_PLAYER_SIDE);
		return StageHelper::CheckHeroMinionDead(board);
	}

	Card draw_card = board.player_deck.Draw();

	if (board.player_hand.GetCount() < 10) {
		board.player_hand.AddCard(draw_card);
	}
	else {
		// hand can have maximum of 10 cards
		// TODO: distroy card (trigger deathrattle?)
	}

	return false;
}

inline bool StageHelper::OpponentDrawCard(Board & board)
{
	if (board.opponent_cards.GetDeckCount() == 0) {
		StageHelper::Fatigue(board, SLOT_OPPONENT_SIDE);
		return StageHelper::CheckHeroMinionDead(board);
	}

	if (board.opponent_cards.GetHandCount() < 10) {
		board.opponent_cards.DrawFromDeckToHand();
	}
	else {
		// hand can have maximum of 10 cards
		// TODO: distroy card (trigger deathrattle?)
	}

	return false;
}

inline void StageHelper::DealDamage(GameEngine::Board & board, SlotIndex taker_idx, int damage)
{
	if (SlotIndexHelper::IsHero(taker_idx)) {
		auto target = board.object_manager.GetObject(taker_idx);
		auto target_hero = dynamic_cast<BoardObjects::Hero *>(target);
		if (!target_hero) throw std::runtime_error("consistency check failed");
		return StageHelper::DealDamage(board, target_hero, damage);
	}
	else {
		auto target_minion = board.object_manager.GetMinionIteratorWithIndexFromSlotIndex(taker_idx);
		return StageHelper::DealDamage(board, target_minion, damage);
	}
}

inline SlotIndex StageHelper::GetTargetForForgetfulAttack(GameEngine::Board & board, SlotIndex origin_attacked)
{
	const bool player_side = SlotIndexHelper::IsPlayerSide(origin_attacked);

	int possible_targets = 1 - 1; // +1 for the hero, -1 for the original attack target
	if (player_side) possible_targets += board.object_manager.GetPlayerMinionsCount();
	else possible_targets += board.object_manager.GetOpponentMinionsCount();

	if (possible_targets == 0)
	{
		// no other target available --> attack original target
		return origin_attacked;
	}

	if (board.random_generator.GetRandom() % 2 == 0) {
		// forgetful effect does not triggered
		return origin_attacked;
	}

	int r = board.random_generator.GetRandom() % possible_targets;
	
	if (player_side) {
		if (origin_attacked != SLOT_PLAYER_HERO) {
			// now we have a chance to attack the hero, take care of it first
			if (r == possible_targets - 1) {
				return SLOT_PLAYER_HERO;
			}
		}
		// now we can only hit the minions
		return SlotIndexHelper::GetPlayerMinionIndex(r);
	}
	else {
		if (origin_attacked != SLOT_OPPONENT_HERO) {
			// now we have a chance to attack the hero, take care of it first
			if (r == possible_targets - 1) {
				return SLOT_OPPONENT_HERO;
			}
		}
		// now we can only hit the minions
		return SlotIndexHelper::GetOpponentMinionIndex(r);
	}
}

inline void StageHelper::DealDamage(GameEngine::Board & board, BoardObjects::Hero * target, int damage)
{
	target->TakeDamage(damage);
}

inline void StageHelper::DealDamage(GameEngine::Board & board, BoardObjects::MinionsIteratorWithIndex & target, int damage)
{
	target->TakeDamage(damage);

	target->HookMinionCheckEnraged(board, target);
}

inline void StageHelper::HandleAttack(GameEngine::Board & board, GameEngine::SlotIndex attacker_idx, GameEngine::SlotIndex attacked_idx)
{
	// TODO: trigger secrets

	GameEngine::BoardObjects::ObjectBase * attacker = board.object_manager.GetObject(attacker_idx);
	
	if (attacker->IsForgetful())
	{
		attacked_idx = StageHelper::GetTargetForForgetfulAttack(board, attacked_idx);
	}

	GameEngine::BoardObjects::ObjectBase * attacked = board.object_manager.GetObject(attacked_idx);

	StageHelper::DealDamage(board, attacked_idx, attacker->GetAttack());
	if (!SlotIndexHelper::IsHero(attacked_idx))
	{
		// If hero, no damage to the attacker
		// If minion, deal damage equal to the attacked's attack
		StageHelper::DealDamage(board, attacker_idx, attacked->GetAttack());
	}
	
	attacker->AttackedOnce();

	if (attacker->IsFreezeAttacker()) attacked->SetFreezed(true);
	if (attacked->IsFreezeAttacker()) attacker->SetFreezed(true);
}

inline void GameEngine::StageHelper::RemoveMinionsIfDead(Board & board, SlotIndex side)
{
	std::list<std::function<void(Board &)>> death_triggers;

	while (true) { // loop until no deathrattle are triggered
		death_triggers.clear();

		// mark as pending death
		for (auto it = board.object_manager.GetMinionIteratorWithIndexForSide(side); !it.IsEnd();)
		{
			if (it.IsPendingRemoval()) {
				it.GoToNext();
				continue;
			}

			if (it->GetHP() > 0) {
				it.GoToNext();
				continue;
			}

			for (auto const& trigger : it->MoveOutOnDeathTriggers())
			{
				death_triggers.push_back(std::bind(trigger, std::placeholders::_1, it));
			}

			it.MarkPendingRemoval();
		}

		// trigger deathrattles
		for (auto const& death_trigger : death_triggers)
		{
			death_trigger(board);
		}

		// actually remove died minions
		for (auto it = board.object_manager.GetMinionIteratorWithIndexForSide(side); !it.IsEnd();)
		{
			if (it.IsPendingRemoval()) 
			{
				// remove all effects (including auras)
				it->ClearEnchantments();
				it->ClearAuras();

				it.EraseAndGoToNext();
			}
			else {
				it.GoToNext();
			}
		}

		if (death_triggers.empty()) break;
	}
}

inline bool GameEngine::StageHelper::CheckHeroMinionDead(Board & board)
{
	if (board.object_manager.GetPlayerHero()->GetHP() <= 0) {
		board.stage = STAGE_LOSS;
		return true;
	}

	if (board.object_manager.GetOpponentHero()->GetHP() <= 0) {
		board.stage = STAGE_WIN;
		return true;
	}

	RemoveMinionsIfDead(board, SLOT_PLAYER_SIDE);
	RemoveMinionsIfDead(board, SLOT_OPPONENT_SIDE);

	return false; // state not changed
}

inline void StageHelper::Fatigue(GameEngine::Board & board, SlotIndex side)
{
	if (SlotIndexHelper::IsPlayerSide(side)) {
		++board.player_stat.fatigue_damage;
		StageHelper::DealDamage(board, board.object_manager.GetPlayerHero(), board.player_stat.fatigue_damage);
	}
	else {
		++board.opponent_stat.fatigue_damage;
		StageHelper::DealDamage(board, board.object_manager.GetOpponentHero(), board.opponent_stat.fatigue_damage);
	}
}

} // namespace GameEngine
#endif
