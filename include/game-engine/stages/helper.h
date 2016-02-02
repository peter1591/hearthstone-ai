#ifndef STAGES_HELPER_H
#define STAGES_HELPER_H

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/targetor.h"

namespace GameEngine {

class StageHelper
{
public: // return true if game state changed (e.g., win/loss)
	static bool PlayerDrawCard(Board &board);
	static bool OpponentDrawCard(Board &board);

	// check if one of the heros dead, and move to the corresponding stage
	// return true if game ends (i.e., state changed)
	static bool CheckWinLoss(Board &board);

public:
	// handle minion/hero attack, calculate damages
	static void HandleAttack(GameEngine::Board & board, int attacker_idx, int attacked_idx);

	static void TakeDamage(GameEngine::Board & board, int taker_idx, int damage);

private:
	static Minions::container_type::iterator GetMinionIterator(GameEngine::Board & board, int pos, Minions * & container);
	static void TakeDamage(Minions::container_type::iterator taker, int damage);

	// return true if dead
	static bool RemoveDeadMinion(Minions::container_type &minions, Minions::container_type::iterator it);

	static void Fatigue(PlayerStat &player_stat);
};

inline bool StageHelper::PlayerDrawCard(Board & board)
{
	if (board.player_deck.GetCards().empty()) {
		StageHelper::Fatigue(board.player_stat);
		return StageHelper::CheckWinLoss(board);
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
		return StageHelper::CheckWinLoss(board);
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

	return container->GetMinions().begin() + minion_idx;
}

inline void StageHelper::TakeDamage(GameEngine::Board & board, int taker_idx, int damage)
{
	if (taker_idx == Targetor::GetPlayerHeroIndex()) {
		board.player_stat.hp -= damage;
	}
	else if (taker_idx == Targetor::GetOpponentHeroIndex()) {
		board.opponent_stat.hp -= damage;
	}

	Minions * container = nullptr;
	Minions::container_type::iterator taker = GetMinionIterator(board, taker_idx, container);
	return TakeDamage(taker, damage);
}

inline void StageHelper::TakeDamage(Minions::container_type::iterator taker, int damage)
{
	taker->TakeDamage(damage);
}

inline void StageHelper::HandleAttack(GameEngine::Board & board, int attacker_idx, int attacked_idx)
{
	// TODO: trigger secrets

	if (attacker_idx == Targetor::GetPlayerHeroIndex() ||
		attacker_idx == Targetor::GetOpponentHeroIndex())
	{
		// TODO: attacker is hero

	}
	else  
	{
		Minions * attacker_container = nullptr;
		Minions::container_type::iterator attacker = GetMinionIterator(board, attacker_idx, attacker_container);

		if (attacked_idx == Targetor::GetPlayerHeroIndex()) {
			board.player_stat.hp -= attacker->GetAttack();
			attacker->AttackedOnce();
		} else if (attacked_idx == Targetor::GetOpponentHeroIndex()) {
			board.opponent_stat.hp -= attacker->GetAttack();
			attacker->AttackedOnce();
		} else {
			Minions * attacked_container = nullptr;
			Minions::container_type::iterator attacked = GetMinionIterator(board, attacked_idx, attacked_container);

			TakeDamage(attacked, attacker->GetAttack());
			TakeDamage(attacker, attacked->GetAttack());

			// check minion dead
			if (StageHelper::RemoveDeadMinion(attacker_container->GetMinions(), attacker) == false) {
				attacker->AttackedOnce();
			}
			StageHelper::RemoveDeadMinion(attacked_container->GetMinions(), attacked);
		}
	}
}

// return true if player's dead
inline bool StageHelper::CheckWinLoss(Board &board)
{
	if (UNLIKELY(board.player_stat.hp <= 0)) {
		board.stage = STAGE_LOSS;
		return true;
	}

	if (UNLIKELY(board.opponent_stat.hp <= 0)) {
		board.stage = STAGE_WIN;
		return true;
	}

	return false;
}

// return true if dead
inline bool StageHelper::RemoveDeadMinion(Minions::container_type &minions, Minions::container_type::iterator it)
{
	if (it->GetHP() > 0) return false;

	minions.erase(it);
	return true;
}

inline void StageHelper::Fatigue(PlayerStat & player_stat)
{
	++player_stat.fatigue_damage;
	player_stat.hp -= player_stat.fatigue_damage;
}

} // namespace GameEngine
#endif
