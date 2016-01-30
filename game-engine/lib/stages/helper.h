#ifndef STAGES_HELPER_H
#define STAGES_HELPER_H

#include "stages/common.h"
#include "game-engine/board.h"

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
	static void HandleAttack(
		PlayerStat &attacker_hero,
		Minions::container_type &attacker_minions,
		int attacker_idx, // -1 for the hero
		PlayerStat &attacked_hero,
		Minions::container_type &attacked_minions,
		int attacked_idx);

private:
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

	if (board.player_hand.GetCards().size() < 10) {
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

inline void StageHelper::HandleAttack(
		PlayerStat &attacker_hero,
		Minions::container_type &attacker_minions,
		int attacker_idx, // -1 for the hero
		PlayerStat &attacked_hero,
		Minions::container_type &attacked_minions,
		int attacked_idx)
{
	// TODO: trigger secrets

	if (attacker_idx < 0) {
		// TODO: attacker is hero
		(void)attacker_hero;

	} else {
		Minions::container_type::iterator attacker = attacker_minions.begin() + attacker_idx;

		if (attacked_idx < 0) {
			// target is hero
			attacked_hero.hp -= attacker->GetAttack();
			attacker->AttackedOnce();

		} else {
			Minions::container_type::iterator attacked = attacked_minions.begin() + attacked_idx;

			attacked->TakeDamage(attacker->GetAttack());
			attacker->TakeDamage(attacked->GetAttack());

			// check minion dead
			if (StageHelper::RemoveDeadMinion(attacker_minions, attacker) == false) {
				attacker->AttackedOnce();
			}
			StageHelper::RemoveDeadMinion(attacked_minions, attacked);
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
