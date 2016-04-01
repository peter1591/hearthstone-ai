#ifndef STAGES_PLAYER_CHOOSE_BOARD_MOVE_H
#define STAGES_PLAYER_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>
#include <memory>

#include "game-engine/stages/common.h"
#include "game-engine/stages/weighted-moves.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

class StagePlayerChooseBoardMove
{
	public:
		static const Stage stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;

		static void GetNextMoves(
			const Board &board, NextMoveGetter &next_move_getter, bool & is_deterministic)
		{
			StageHelper::GetBoardMoves(board, SlotIndex::SLOT_PLAYER_SIDE, board.player, next_move_getter, is_deterministic);
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
			constexpr int weight_equip_weapon = 100;
			constexpr int weight_attack = 100;

			moves.Clear();

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			moves.AddMove(move, weight_end_turn);

			// the choices to play a card from hand
			bool can_play_minion = !board.player.minions.IsFull();
			SlotIndexBitmap required_targets;
			bool meet_requirements;
			for (size_t hand_idx = 0; hand_idx < board.player.hand.GetCount(); ++hand_idx)
			{
				Card const& playing_card = board.player.hand.GetCard(hand_idx);
				switch (playing_card.type) {
				case Card::TYPE_MINION:
					if (!can_play_minion) continue;
					if (board.player.stat.crystal.GetCurrent() < playing_card.cost) continue;

					if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, board, SLOT_PLAYER_SIDE, required_targets, meet_requirements)
						&& meet_requirements == false)
					{
						break;
					}

					move.action = Move::ACTION_PLAYER_PLAY_MINION;
					move.data.player_play_minion_data.hand_card = hand_idx;
					move.data.player_play_minion_data.card_id = playing_card.id;
					move.data.player_play_minion_data.data.put_location = SlotIndexHelper::GetPlayerMinionIndex(board.player.minions.GetMinionCount());
					if (required_targets.None()) move.data.player_play_minion_data.data.target = SLOT_INVALID;
					else move.data.player_play_minion_data.data.target = required_targets.GetOneTarget();
					
					moves.AddMove(move, weight_play_minion);
					break;

				case Card::TYPE_WEAPON:
					if (board.player.stat.crystal.GetCurrent() < playing_card.cost) continue;

					if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, board, SLOT_PLAYER_SIDE, required_targets, meet_requirements)
						&& meet_requirements == false)
					{
						break;
					}

					move.action = Move::ACTION_PLAYER_EQUIP_WEAPON;
					move.data.player_equip_weapon_data.hand_card = hand_idx;
					move.data.player_equip_weapon_data.card_id = playing_card.id;
					if (required_targets.None()) move.data.player_equip_weapon_data.data.target = SLOT_INVALID;
					else move.data.player_equip_weapon_data.data.target = required_targets.GetOneTarget();

					moves.AddMove(move, weight_play_minion);
					break;

				default:
					break; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			SlotIndexBitmap attacker;
			SlotIndexBitmap attacked;

			attacker = SlotIndexHelper::GetTargets(SlotIndexHelper::TARGET_TYPE_PLAYER_ATTACKABLE, board);

			if (!attacker.None()) {
				attacked = SlotIndexHelper::GetTargets(SlotIndexHelper::TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED, board);

				while (!attacker.None()) {
					SlotIndex attacker_idx = attacker.GetOneTarget();
					attacker.ClearOneTarget(attacker_idx);

					while (!attacked.None()) {
						SlotIndex attacked_idx = attacked.GetOneTarget();
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

				case Move::ACTION_PLAYER_EQUIP_WEAPON:
					return StagePlayerChooseBoardMove::EquipWeapon(board, move);

				case Move::ACTION_ATTACK:
					return StagePlayerChooseBoardMove::PlayerAttack(board, move);

				case Move::ACTION_END_TURN:
					return StagePlayerChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void PlayMinion(Board &board, const Move &move)
		{
			board.data.player_play_minion_data = move.data.player_play_minion_data;
			board.stage = STAGE_PLAYER_PUT_MINION;
		}

		static void EquipWeapon(Board &board, const Move &move)
		{
			board.data.player_equip_weapon_data = move.data.player_equip_weapon_data;
			board.stage = STAGE_PLAYER_EQUIP_WEAPON;
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
