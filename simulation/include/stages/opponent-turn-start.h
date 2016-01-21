#ifndef STAGES_OPPONENT_TURN_START_H
#define STAGES_OPPONENT_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "stages/common.h"
#include "board.h"

class StageOpponentTurnStart
{
	public:
		static const Stage stage = STAGE_OPPONENT_TURN_START;

		static void Go(Board &board)
		{
			board.opponent_stat.crystal.TurnStart();

			// draw a card
			if (board.player_deck.GetCards().empty()) {
				// no any card can draw, take damage
				// TODO
				board.stage = STAGE_LOSS;
				return;
			} else {
				if (board.opponent_cards.GetHandCount() < 10) {
					board.opponent_cards.DrawFromDeckToHand();
				} else {
					// hand can have maximum of 10 cards
					// TODO: distroy card (trigger deathrattle?)
				}
			}

			// reset minion stat
			for (auto &minion : board.opponent_minions.GetMinions()) {
				minion.TurnStart();
			}

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
	}
};

#endif
