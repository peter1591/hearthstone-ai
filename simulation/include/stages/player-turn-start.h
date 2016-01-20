#ifndef STAGES_PLAYER_TURN_START_H
#define STAGES_PLAYER_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "stages/common.h"
#include "board.h"

class StagePlayerTurnStart
{
	public:
		static const Stage stage = STAGE_PLAYER_TURN_START;

		static void Go(Board &board)
		{
			board.player_stat.crystal.TurnStart();

			// draw a card
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

			for (auto &minion : board.player_minions.GetMinions()) {
				minion.TurnStart();
			}

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
	}
};

#endif
