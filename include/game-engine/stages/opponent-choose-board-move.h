#ifndef STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H
#define STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"

namespace GameEngine {

class StageOpponentChooseBoardMove
{
	public:
		static const Stage stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;

		static void GetNextMoves(const Board &board, NextMoveGetter &next_move_getter)
		{
			bool can_play_minion = !board.opponent_minions.IsFull();

			std::vector<Card> playable_minions;
			if (can_play_minion) {
				board.opponent_cards.GetPossiblePlayableMinions(board.opponent_stat, playable_minions);
			}

			// the choices to play a card from hand
			if (can_play_minion) {
				for (const auto &card : playable_minions) {
					StageOpponentChooseBoardMove::GetNextMoves_PlayMinion(card, board, next_move_getter);
				}
			}

			// the choices to attack by hero/minion
			TargetorBitmap attacker;
			TargetorBitmap attacked;

			// TODO: check if player has weapon
			attacker = Targetor::GetTargets(Targetor::TARGET_TYPE_OPPONENT_ATTACKABLE, board);

			if (!attacker.None()) {
				attacked = Targetor::GetTargets(Targetor::TARGET_TYPE_PLAYER_CAN_BE_ATTACKED, board);

				NextMoveGetter::ItemAttack attack_move(std::move(attacker), std::move(attacked));
				next_move_getter.AddItem(std::move(attack_move));
			}

			// the choice to end turn
			Move end_turn_move;
			end_turn_move.action = Move::ACTION_END_TURN;
			next_move_getter.AddItem(std::move(end_turn_move));
		}

		static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
		{
			// heuristic goes here
			// 1. play minion is good (always put minion at the right-most)
			// 2. minion attack is good
			// 3. hero attack is good
			// 4. effective spell is good

			thread_local static std::unique_ptr<WeightedMoves> ptr_moves = nullptr;
			if (!ptr_moves) ptr_moves.reset(new WeightedMoves());

			WeightedMoves & moves = *ptr_moves; // do not re-allocate at each call
			Move move;

			constexpr int weight_end_turn = 1;
			constexpr int weight_play_minion = 100;
			constexpr int weight_attack = 100;

			moves.Clear();

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			moves.AddMove(move, weight_end_turn);

			// the choices to play a card from hand
			std::vector<Card> playable_minions;
			bool can_play_minion = !board.opponent_minions.IsFull();
			board.opponent_cards.GetPossiblePlayableMinions(board.opponent_stat, playable_minions);
			if (can_play_minion) {
				for (const auto &card : playable_minions) {
					move.action = Move::ACTION_OPPONENT_PLAY_MINION;
					move.data.opponent_play_minion_data.card = card;
					move.data.opponent_play_minion_data.data.put_location = Targetor::GetOpponentMinionIndex((int)board.opponent_minions.GetMinionCount());
					moves.AddMove(move, weight_play_minion);
				}
			}

			// the choices to attack by hero/minion
			TargetorBitmap attacker;
			TargetorBitmap attacked;

			attacker = Targetor::GetTargets(Targetor::TARGET_TYPE_OPPONENT_ATTACKABLE, board);

			if (!attacker.None()) {
				attacked = Targetor::GetTargets(Targetor::TARGET_TYPE_PLAYER_CAN_BE_ATTACKED, board);
				
				while (!attacker.None()) {
					int attacker_idx = attacker.GetOneTarget();
					attacker.ClearOneTarget(attacker_idx);

					while (!attacked.None()) {
						int attacked_idx = attacked.GetOneTarget();
						attacked.ClearOneTarget(attacked_idx);

						move.action = Move::ACTION_ATTACK;
						move.data.attack_data.attacker_idx = attacker_idx;
						move.data.attack_data.attacked_idx = attacked_idx;
						moves.AddMove(move, weight_attack);
					} // for attacked
				} // for attacker
			}

			if (moves.Choose(rand, good_move) == false) {
				throw std::runtime_error("no action is available (should not happen)");
			}

			return;
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_OPPONENT_PLAY_MINION:
					return StageOpponentChooseBoardMove::PlayHandCardMinion(board, move);
				case Move::ACTION_ATTACK:
					return StageOpponentChooseBoardMove::OpponentAttack(board, move);
				case Move::ACTION_END_TURN:
					return StageOpponentChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void GetNextMoves_PlayMinion(const Card &card, const Board &board, NextMoveGetter &next_move_getter)
		{
			Move move;

			move.action = Move::ACTION_OPPONENT_PLAY_MINION;
			move.data.opponent_play_minion_data.card = card;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (int i = 0; i <= (int)board.opponent_minions.GetMinionCount(); ++i) {
				move.data.opponent_play_minion_data.data.put_location = Targetor::GetOpponentMinionIndex(i);
				next_move_getter.AddItem(move);
			}
#else
			move.data.opponent_play_minion_data.data.put_location = Targetor::GetOpponentMinionIndex((int)board.opponent_minions.GetMinionCount());
			next_move_getter.AddItem(move);
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			board.data.opponent_play_minion_data = move.data.opponent_play_minion_data;
			board.stage = STAGE_OPPONENT_PUT_MINION;
		}

		static void GetNextMoves_Attack(int attacker_idx, int attacked_idx, std::list<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_ATTACK;
			move.data.attack_data.attacker_idx = attacker_idx;
			move.data.attack_data.attacked_idx = attacked_idx;

			next_moves.push_back(move);
		}

		static void OpponentAttack(Board &board, const Move &move)
		{
			board.data.attack_data = move.data.attack_data;
			board.stage = STAGE_OPPONENT_ATTACK;
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = STAGE_OPPONENT_TURN_END;
		}
};

} // namespace GameEngine

#endif
