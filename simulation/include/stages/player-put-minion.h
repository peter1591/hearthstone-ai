#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;
		static std::string GetStageStringName() { return "StagePlayerPutMinion"; }

		static void GetNextMoves(const Board &, std::vector<Move> &next_moves)
		{
			return StageCommonUtilities::GetGameFlowMove(next_moves);
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			Minion minion;
			const Board::PlayerPutMinionData &data = board.data.player_put_minion_data;

#ifdef ENABLE_DEBUG_CHECKS
			if (move.action != Move::ACTION_GAME_FLOW) throw std::runtime_error("Invalid move");
#endif

			std::vector<Card>::const_iterator it_hand_card = board.player_hand.GetCards().begin() + data.idx_hand_card;

			board.player_stat.crystal.CostCrystals(it_hand_card->cost);

			// TODO: handle battlecry
			minion.card_id = it_hand_card->id;
			minion.max_hp = it_hand_card->data.minion.hp;
			minion.hp = minion.max_hp;
			minion.attack = it_hand_card->data.minion.attack;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.player_minions.AddMinion(minion, data.location);
#else
			board.player_minions.AddMinion(minion); // add to the rightmost
#endif

			board.player_hand.RemoveCard(data.idx_hand_card);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

#endif
