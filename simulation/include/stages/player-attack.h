#ifndef STAGES_PLAYER_ATTACK_H
#define STAGES_PLAYER_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StagePlayerAttack
{
	public:
		static const Stage stage = STAGE_PLAYER_ATTACK;

		static void Go(Board &board)
		{
			const Board::PlayerAttackData &data = board.data.player_attack_data;

			// TODO: trigger secrets

			// TODO: handle attack
			if (data.attacker_idx < 0) {
				// TODO: attacker is hero

			} else {
				Minions::container_type::iterator attacker = board.player_minions.GetMinions().begin() + data.attacker_idx;

				if (data.attacked_idx < 0) {
					// target is hero
					board.opponent_stat.hp -= attacker->GetAttack();
					if (UNLIKELY(CheckOpponentDead(board))) return;
					attacker->AttackedOnce();

				} else {
					Minions::container_type::iterator attacked = board.opponent_minions.GetMinions().begin() + data.attacked_idx;

					attacked->TakeDamage(attacker->GetAttack());
					attacker->TakeDamage(attacked->GetAttack());

					// check minion dead
					if (CheckMinionDead(board.player_minions, attacker) == false) {
						attacker->AttackedOnce();
					}
					CheckMinionDead(board.opponent_minions, attacked);
				}
			}

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
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
