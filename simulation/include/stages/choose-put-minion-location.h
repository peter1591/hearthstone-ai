#ifndef _STAGES_CHOOSE_PUT_MINION_LOCATION_H
#define _STAGES_CHOOSE_PUT_MINION_LOCATION_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageChoosePutMinionLocation
{
	public:
		static const bool is_random_node = false;

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			size_t total_minions = board.player_minions.GetMinions().size();
			Move move;

			next_moves.clear();
			next_moves.reserve(1 + total_minions);

			move.action = Move::ACTION_CHOOSE_PUT_MINION_LOCATION;

			for (size_t i=0; i<=total_minions; ++i) {
				move.data.choose_put_minion_location_data.put_location = i;
				next_moves.push_back(move);
			}
		}

		static void ApplyMove(Board &board, const Move &move)
		{
#ifdef ENABLE_DEBUG_CHECKS
			if (move.action != Move::ACTION_CHOOSE_PUT_MINION_LOCATION)
			{
				throw std::runtime_error("Invalid move for StageChoosePutMinionLocation");
			}
#endif

			const Move::ChoosePutMinionLocationData &data = move.data.choose_put_minion_location_data;

			Minion minion;
			std::vector<Card>::iterator it_hand_card = board.data.play_minion_data.it_hand_card;

			// TODO: handle battlecry
			minion.card_id = it_hand_card->id;
			minion.max_hp = it_hand_card->data.minion.hp;
			minion.hp = minion.max_hp;
			minion.attack = it_hand_card->data.minion.attack;

			board.player_minions.AddMinion(minion, data.put_location);

			board.player_hand.GetCards().erase(it_hand_card);

			board.state.Set(BoardState::STAGE_CHOOSE_BOARD_MOVE);
	}
};

#endif
