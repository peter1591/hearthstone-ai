#ifndef _STAGES_START_TURN_H
#define _STAGES_START_TURN_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageStartTurn
{
	public:
		static const bool is_random_node = true;

		static void GetNextMoves(const Board &, std::vector<Move> &next_moves)
		{
			return StageCommonUtilities::GetGameFlowMove(next_moves);
		}

		static void ApplyMove(Board &board, const Move &move)
		{
#ifdef ENABLE_DEBUG_CHECKS
			if (move.action != Move::ACTION_GAME_FLOW)
			{
				throw std::runtime_error("Invalid move for StageStartTurn::ApplyMove()");
			}
#endif

			if (board.player_deck.GetCards().empty()) {
				// no any card can draw, take damage
				// TODO
				board.GetState().Set(BoardState::STAGE_WIN);
				return;
			} else {
				Card draw_card = board.player_deck.Draw();

				if (board.player_hand.GetCards().size() < 10) {
					board.player_hand.AddCard(draw_card);
				} else {
					// hand can have maximum of 10 cards
					// TODO: distroy card (trigger deathrattle?)
				}
			}
			board.GetState().Set(BoardState::STAGE_CHOOSE_BOARD_MOVE);
	}
};

#endif
