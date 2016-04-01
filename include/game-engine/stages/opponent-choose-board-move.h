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

		static void GetNextMoves(const Board &board, NextMoveGetter &next_move_getter, bool & is_deterministic)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
		}

		static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
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
		static void GetNextMoves_PlayMinion(const Card &playing_card, const Board &board, NextMoveGetter &next_move_getter)
		{
			if (board.opponent_stat.crystal.GetCurrent() < playing_card.cost) return;

			SlotIndexBitmap required_targets;
			bool meet_requirements;
			if (Cards::CardCallbackManager::GetRequiredTargets(playing_card.id, board, SLOT_OPPONENT_SIDE, required_targets, meet_requirements) &&
				meet_requirements == false)
			{
				return;
		}

			// TODO: check play requirements

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (int i = 0; i <= board.object_manager.opponent_minions.GetMinionCount(); ++i)
			{
				SlotIndex idx = SlotIndexHelper::GetOpponentMinionIndex(i);
				next_move_getter.AddItem(NextMoveGetter::ItemOpponentPlayMinion(playing_card, idx, required_targets));
			}
#else
			next_move_getter.AddItem(NextMoveGetter::ItemOpponentPlayMinion(playing_card, SlotIndexHelper::GetOpponentMinionIndex(board.object_manager.opponent_minions.GetMinionCount()), required_targets));
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			board.data.opponent_play_minion_data = move.data.opponent_play_minion_data;
			board.stage = STAGE_OPPONENT_PUT_MINION;
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
