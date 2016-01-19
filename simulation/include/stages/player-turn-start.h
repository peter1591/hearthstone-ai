#ifndef _STAGES_PLAYER_TURN_START_H
#define _STAGES_PLAYER_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "stages/common.h"
#include "board.h"

class StagePlayerTurnStart
{
	public:
		static const Stage stage = STAGE_PLAYER_TURN_START;
		static const bool is_random_node = true;
		static const bool is_player_turn = true;
		static std::string GetStageStringName() { return "StagePlayerTurnStart"; }

		static void GetNextMoves(const Board &, std::vector<Move> &next_moves)
		{
			return StageCommonUtilities::GetGameFlowMove(next_moves);
		}

		static void ApplyMove(Board &board, const Move &move)
		{
#ifdef ENABLE_DEBUG_CHECKS
			if (move.action != Move::ACTION_GAME_FLOW)
			{
				throw std::runtime_error("Invalid move for StagePlayerTurnStart::ApplyMove()");
			}
#endif

			if (board.player_deck.GetCards().empty()) {
				// no any card can draw, take damage
				// TODO
				board.stage = STAGE_WIN;
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
			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
	}
};

#endif
