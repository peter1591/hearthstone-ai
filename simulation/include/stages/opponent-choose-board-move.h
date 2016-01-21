#ifndef STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H
#define STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageOpponentChooseBoardMove
{
	public:
		static const Stage stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			size_t guessed_next_moves;
			bool can_play_minion = !board.opponent_minions.IsFull();
			Move move;
			std::vector<Card> playable_minions;

			guessed_next_moves = 1; // end turn

			// for play minions
			if (can_play_minion) {
				board.opponent_cards.GetPossiblePlayableMinions(board.opponent_stat, playable_minions);
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				guessed_next_moves += playable_minions.size() * (board.opponent_minions.GetMinions().size()+1);
#else
				guessed_next_moves += playable_minions.size();
#endif
			}

			// for attack
			guessed_next_moves += (board.opponent_minions.GetMinions().size()+1) * (board.opponent_minions.GetMinions().size()+1);

			next_moves.reserve(guessed_next_moves);

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			next_moves.push_back(move);

			// the choices to play a card from hand
			if (can_play_minion) {
				for (const auto &card : playable_minions) {
					StageOpponentChooseBoardMove::GetNextMoves_PlayMinion(card, board, next_moves);
				}
			}

			// the choices to attack by hero/minion
			for (int attacker_idx = -1; attacker_idx < (int)board.opponent_minions.GetMinions().size(); attacker_idx++) {
				if (attacker_idx == -1) {
					continue; // TODO: check if opponenthas weapon
				} else {
					if (!board.opponent_minions.GetMinions()[attacker_idx].Attackable()) continue;
				}

				for (int attacked_idx = -1; attacked_idx < (int)board.player_minions.GetMinions().size(); attacked_idx++) {
					StageOpponentChooseBoardMove::GetNextMoves_Attack(attacker_idx, attacked_idx, next_moves);
				}
			}
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_OPPONENT_PLAY_MINION:
					return StageOpponentChooseBoardMove::PlayHandCardMinion(board, move);
				case Move::ACTION_OPPONENT_ATTACK:
					return StageOpponentChooseBoardMove::OpponentAttack(board, move);
				case Move::ACTION_END_TURN:
					return StageOpponentChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void GetNextMoves_PlayMinion(const Card &card, const Board &board, std::vector<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_OPPONENT_PLAY_MINION;
			move.data.opponent_play_minion_data.card = card;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (size_t i=0; i<=board.opponent_minions.GetMinions().size(); ++i) {
				move.data.opponent_play_minion_data.location = i;
				next_moves.push_back(move);
			}
#else
			(void)board;
			next_moves.push_back(move);
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			const Move::OpponentPlayMinionData &data = move.data.opponent_play_minion_data;
			board.data.opponent_put_minion_data.card = data.card;
			board.data.opponent_put_minion_data.location = data.location;
			board.stage = STAGE_OPPONENT_PUT_MINION;
		}

		static void GetNextMoves_Attack(int attacker_idx, int attacked_idx, std::vector<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_OPPONENT_ATTACK;
			move.data.opponent_attack_data.attacker_idx = attacker_idx;
			move.data.opponent_attack_data.attacked_idx = attacked_idx;

			next_moves.push_back(move);
		}

		static void OpponentAttack(Board &board, const Move &move)
		{
			const Move::OpponentAttackData &data = move.data.opponent_attack_data;

			board.data.opponent_attack_data.attacker_idx = data.attacker_idx;
			board.data.opponent_attack_data.attacked_idx = data.attacked_idx;

			board.stage = STAGE_OPPONENT_ATTACK;
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = STAGE_OPPONENT_TURN_END;
		}
};

#endif
