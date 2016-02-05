#ifndef STAGES_HELPER_H
#define STAGES_HELPER_H

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/targetor.h"

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
	static bool PlayMinion(Board & board, Card const& card, PlayMinionData const& data);

	// no battle cry
	static bool SummonMinion(Board & board, Card const& card, PlayMinionData const& data);

public:
	static Minions::container_type::iterator GetMinionIterator(GameEngine::Board & board, int pos, Minions * & container);

	// handle minion/hero attack, calculate damages
	static void HandleAttack(GameEngine::Board & board, int attacker_idx, int attacked_idx);

	static void TakeDamage(GameEngine::Board & board, int taker_idx, int damage);

private:
	static int GetNewAttackedTargetForForgetfulAttack(GameEngine::Board & board, int origin_attacked);

	static void RemoveMinionsIfDead(Board & board, Minions & minions);

	static void Fatigue(PlayerStat &player_stat);
};

inline bool StageHelper::PlayerDrawCard(Board & board)
{
	if (board.player_deck.GetCards().empty()) {
		StageHelper::Fatigue(board.player_stat);
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
		StageHelper::Fatigue(board.opponent_stat);
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

inline Minions::container_type::iterator StageHelper::GetMinionIterator(GameEngine::Board & board, int pos, Minions * & container)
{
	int minion_idx;
	if (Targetor::IsPlayerMinion(pos, minion_idx)) {
		container = &board.player_minions;
	}
	else if (Targetor::IsOpponentMinion(pos, minion_idx)) {
		container = &board.opponent_minions;
	}
	else {
		throw std::runtime_error("logic error");
	}

	return container->MinionsBegin() + minion_idx;
}

inline void StageHelper::TakeDamage(GameEngine::Board & board, int taker_idx, int damage)
{
	if (taker_idx == Targetor::GetPlayerHeroIndex()) {
		board.player_stat.hp -= damage;
		return;
	}

	if (taker_idx == Targetor::GetOpponentHeroIndex()) {
		board.opponent_stat.hp -= damage;
		return;
	}

	Minions * container = nullptr;
	Minions::container_type::iterator taker = GetMinionIterator(board, taker_idx, container);
	taker->TakeDamage(damage);
}

inline int StageHelper::GetNewAttackedTargetForForgetfulAttack(GameEngine::Board & board, int origin_attacked)
{
	bool player_side = Targetor::IsPlayerSide(origin_attacked);

	int possible_targets = 1 - 1; // +1 for the hero, -1 for the original attack target
	if (player_side) possible_targets += board.player_minions.GetMinionCount();
	else possible_targets += board.opponent_minions.GetMinionCount();

	if (possible_targets == 0)
	{
		// no other target available --> attack original target
		return origin_attacked;
	}

	int r = board.random_generator.GetRandom() % possible_targets;
	
	if (player_side) {
		if (origin_attacked != Targetor::GetPlayerHeroIndex()) {
			// now we have a chance to attack the hero
			if (r == possible_targets - 1) {
				return Targetor::GetPlayerHeroIndex();
			}
		}
		// now we can only hit the minions
		return Targetor::GetPlayerMinionIndex(r);
	}
	else {
		if (origin_attacked != Targetor::GetOpponentHeroIndex()) {
			// now we have a chance to attack the hero
			if (r == possible_targets - 1) {
				return Targetor::GetOpponentHeroIndex();
			}
		}
		// now we can only hit the minions
		return Targetor::GetOpponentMinionIndex(r);
	}
}

inline void StageHelper::HandleAttack(GameEngine::Board & board, int attacker_idx, int attacked_idx)
{
	// TODO: trigger secrets

	if (attacker_idx == Targetor::GetPlayerHeroIndex() ||
		attacker_idx == Targetor::GetOpponentHeroIndex())
	{
		// TODO: attacker is hero
		throw std::runtime_error("not implemented");
	}
	else  
	{
		Minions * attacker_container = nullptr;
		Minions::container_type::iterator attacker = GetMinionIterator(board, attacker_idx, attacker_container);

		// check if attacker is forgetful
		if (attacker->IsForgetful())
		{
			int r = board.random_generator.GetRandom() % 2;
			if (r == 0) {
				// trigger forgetful effect
				attacked_idx = StageHelper::GetNewAttackedTargetForForgetfulAttack(board, attacked_idx);
			}
		}

		if (attacked_idx == Targetor::GetPlayerHeroIndex()) {
			board.player_stat.hp -= attacker->GetAttack();
			attacker->AttackedOnce();
		} else if (attacked_idx == Targetor::GetOpponentHeroIndex()) {
			board.opponent_stat.hp -= attacker->GetAttack();
			attacker->AttackedOnce();
		} else {
			Minions * attacked_container = nullptr;
			Minions::container_type::iterator attacked = GetMinionIterator(board, attacked_idx, attacked_container);

			attacked->TakeDamage(attacker->GetAttack());
			attacker->TakeDamage(attacked->GetAttack());
		}
	}
}

inline void GameEngine::StageHelper::RemoveMinionsIfDead(Board & board, Minions & minions)
{
	int targetor_idx;

	if (&minions == &board.player_minions) targetor_idx = Targetor::GetPlayerMinionIndex(0);
	else if (&minions == &board.opponent_minions) targetor_idx = Targetor::GetOpponentMinionIndex(0);
	else throw std::runtime_error("consistency check failed");

	while (true) { // loop until no deathrattle are triggered
		std::list<std::function<void(Board &)>> death_triggers;

		for (auto it = minions.MinionsBegin(); it != minions.MinionsEnd();)
		{
			if (it->GetHP() > 0)
			{
				++it;
				++targetor_idx;
				continue;
			}

			for (auto const& trigger : it->MoveOutOnDeathTriggers())
			{
				death_triggers.push_back(std::bind(trigger, std::placeholders::_1, targetor_idx));
			}

			it = minions.MinionsErase(it);
			++targetor_idx;
		}

		// trigger deathrattles
		if (death_triggers.empty()) break;

		for (auto const& death_trigger : death_triggers)
		{
			death_trigger(board);
		}
		death_triggers.clear();
	}
}

inline bool GameEngine::StageHelper::CheckHeroMinionDead(Board & board)
{
	if (UNLIKELY(board.player_stat.hp <= 0)) {
		board.stage = STAGE_LOSS;
		return true;
	}

	if (UNLIKELY(board.opponent_stat.hp <= 0)) {
		board.stage = STAGE_WIN;
		return true;
	}

	RemoveMinionsIfDead(board, board.player_minions);
	RemoveMinionsIfDead(board, board.opponent_minions);

	return false; // state not changed
}

inline void StageHelper::Fatigue(PlayerStat & player_stat)
{
	++player_stat.fatigue_damage;
	player_stat.hp -= player_stat.fatigue_damage;
}

} // namespace GameEngine
#endif
