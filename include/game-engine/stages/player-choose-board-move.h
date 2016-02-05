#ifndef STAGES_PLAYER_CHOOSE_BOARD_MOVE_H
#define STAGES_PLAYER_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>
#include <memory>

#include "game-engine/stages/common.h"
#include "game-engine/stages/weighted-moves.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"
#include "game-engine/targetor.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

class StagePlayerChooseBoardMove
{
	public:
		static const Stage stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;

		static void GetNextMoves(const Board &board, NextMoveGetter &next_move_getter)
		{
			bool const can_play_minion = !board.player_minions.IsFull();

			for (Hand::Locator hand_idx = 0; hand_idx < board.player_hand.GetCount(); ++hand_idx)
			{
				const Card &playing_card = board.player_hand.GetCard(hand_idx);

				switch (playing_card.type) {
				case Card::TYPE_MINION:
					if (!can_play_minion) continue;
					GetNextMove_PlayMinion(board, hand_idx, next_move_getter);

				default:
					continue; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			TargetorBitmap attacker;
			TargetorBitmap attacked;

			// TODO: check if player has weapon
			attacker = Targetor::GetTargets(Targetor::TARGET_TYPE_PLAYER_ATTACKABLE, board);

			if (!attacker.None()) {
				attacked = Targetor::GetTargets(Targetor::TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED, board);

				NextMoveGetter::ItemAttack player_attack_move(std::move(attacker), std::move(attacked));
				next_move_getter.AddItem(std::move(player_attack_move));
			}
			
			// the choice to end turn
			Move move_end_turn;
			move_end_turn.action = Move::ACTION_END_TURN;
			next_move_getter.AddItem(std::move(move_end_turn));
		}

		static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
		{
			// heuristic goes here
			// 1. play minion is good (always put minion to the rightmost)
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
			bool can_play_minion = !board.player_minions.IsFull();
			TargetorBitmap required_targets;
			bool meet_requirements;
			for (size_t hand_idx = 0; hand_idx < board.player_hand.GetCount(); ++hand_idx)
			{
				Card const& playing_card = board.player_hand.GetCard(hand_idx);
				switch (playing_card.type) {
				case Card::TYPE_MINION:
					if (!can_play_minion) continue;
					if (board.player_stat.crystal.GetCurrent() < playing_card.cost) continue;

					if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, board, required_targets, meet_requirements)
						&& meet_requirements == false)
					{
						break;
					}

					move.action = Move::ACTION_PLAYER_PLAY_MINION;
					move.data.player_play_minion_data.hand_card = hand_idx;
					move.data.player_play_minion_data.data.put_location = Targetor::GetPlayerMinionIndex((int)board.player_minions.GetMinionCount());
					if (required_targets.None()) move.data.player_play_minion_data.data.target = 0;
					else move.data.player_play_minion_data.data.target = required_targets.GetOneTarget();

					moves.AddMove(move, weight_play_minion);
				default:
					continue; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			TargetorBitmap attacker;
			TargetorBitmap attacked;

			attacker = Targetor::GetTargets(Targetor::TARGET_TYPE_PLAYER_ATTACKABLE, board);

			if (!attacker.None()) {
				attacked = Targetor::GetTargets(Targetor::TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED, board);

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
				case Move::ACTION_PLAYER_PLAY_MINION:
					return StagePlayerChooseBoardMove::PlayMinion(board, move);

				case Move::ACTION_ATTACK:
					return StagePlayerChooseBoardMove::PlayerAttack(board, move);

				case Move::ACTION_END_TURN:
					return StagePlayerChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void GetNextMove_PlayMinion(Board const& board, Hand::Locator hand_card, NextMoveGetter &next_move_getter)
		{
			const Card &playing_card = board.player_hand.GetCard(hand_card);

			if (board.player_stat.crystal.GetCurrent() < playing_card.cost) return;

			TargetorBitmap required_targets;
			bool meet_requirements;
			if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, board, required_targets, meet_requirements) &&
				meet_requirements == false)
			{
				return;
			}

			// TODO: check play requirements

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (int i = Targetor::GetPlayerMinionIndex(0); i <= Targetor::GetPlayerMinionIndex((int)board.player_minions.GetMinionCount()); ++i)
			{
				next_move_getter.AddItem(NextMoveGetter::ItemPlayerPlayMinion(hand_card, i, required_targets));
			}
#else
			next_move_getter.AddItem(NextMoveGetter::ItemPlayerPlayMinion(hand_card, Targetor::GetPlayerMinionIndex((int)board.player_minions.GetMinionCount()), required_targets));
#endif
		}

		static void PlayMinion(Board &board, const Move &move)
		{
			board.data.player_play_minion_data = move.data.player_play_minion_data;
			board.stage = STAGE_PLAYER_PUT_MINION;
		}

		static void PlayerAttack(Board &board, const Move &move)
		{
			board.data.attack_data = move.data.attack_data;
			board.stage = STAGE_PLAYER_ATTACK;
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = STAGE_PLAYER_TURN_END;
		}
};

} // namespace GameEngine

#endif
