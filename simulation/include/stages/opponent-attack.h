#ifndef STAGES_OPPONENT_ATTACK_H
#define STAGES_OPPONENT_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageOpponentAttack
{
	public:
		static const Stage stage = STAGE_OPPONENT_ATTACK;

		static void Go(Board &board)
		{
			const Board::OpponentAttackData &data = board.data.opponent_attack_data;

			// TODO: trigger player's secrets

			// TODO: handle attack
			if (data.attacker_idx < 0) {
				// TODO: attacker is hero

			} else {
				Minions::container_type::iterator attacker = board.opponent_minions.GetMinions().begin() + data.attacker_idx;

				if (data.attacked_idx < 0) {
					// target is hero
					board.player_stat.hp -= attacker->GetAttack();
					if (UNLIKELY(CheckPlayerDead(board))) return;
					attacker->AttackedOnce();

				} else {
					Minions::container_type::iterator attacked = board.player_minions.GetMinions().begin() + data.attacked_idx;

					attacked->TakeDamage(attacker->GetAttack());
					attacker->TakeDamage(attacked->GetAttack());

					// check minion dead
					if (CheckMinionDead(board.opponent_minions, attacker) == false) {
						attacker->AttackedOnce();
					}
					CheckMinionDead(board.player_minions, attacked);
				}
			}

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}

	private:

		// return true if dead
		static bool CheckMinionDead(Minions &minions, Minions::container_type::iterator it)
		{
			if (it->GetHP() > 0) return false;

			minions.GetMinions().erase(it);
			return true;
		}

		// return true if game ends
		static bool CheckPlayerDead(Board &board)
		{
			if (UNLIKELY(board.player_stat.hp <= 0)) {
				board.stage = STAGE_LOSS;
				return true;
			} else {
				return false;
			}
		}

		// return true if game ends
		static bool CheckOpponentDead(Board &board)
		{
			if (UNLIKELY(board.opponent_stat.hp <= 0)) {
				board.stage = STAGE_WIN;
				return true;

			} else {
				return false;
			}
		}
};

#endif
