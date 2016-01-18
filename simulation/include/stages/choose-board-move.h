#ifndef _STAGES_CHOOSE_BOARD_MOVE_H
#define _STAGES_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageChooseBoardMove
{
	public:
		static const bool is_random_node = false;

		inline static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			bool can_play_minion = !board.player_minions.IsFull();
			Move move;

			next_moves.clear();
			next_moves.reserve(1 + // end turn
					board.player_hand.GetCards().size()); // play a card from hand

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			next_moves.push_back(move);

			// the choices to play a card from hand
			move.action = Move::ACTION_PLAY_HAND_CARD;
			for (size_t hand_idx = 0; hand_idx < board.player_hand.GetCards().size(); ++hand_idx)
			{
				const Card &playing_card = board.player_hand.GetCards()[hand_idx];
				if (playing_card.type == Card::MINION && !can_play_minion) {
					continue;
				}

				move.data.play_hand_card_data.idx = (int)hand_idx;
				next_moves.push_back(move);
			}

			// the choices to attack by hero/minion
			// TODO
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_PLAY_HAND_CARD:
					return StageChooseBoardMove::PlayHandCard(board, move);
				case Move::ACTION_END_TURN:
					return StageChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StageChooseBoardMove");
			}
		}

	private:
		static void PlayHandCard(Board &board, const Move &move)
		{
			const Move::PlayHandCardData &data = move.data.play_hand_card_data;

			std::vector<Card> &hand = board.player_hand.GetCards();
			std::vector<Card>::iterator it_playing_card = hand.begin() + data.idx;

			if (it_playing_card->type == Card::MINION) {
				board.data.play_minion_data.it_hand_card = it_playing_card;
				board.state.Set(BoardState::STAGE_CHOOSE_PUT_MINION_LOCATION);
			} else {
				// TODO: other card types
				throw std::runtime_error("unknown hand card type for Board::DoPlayHandCard()");
			}
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.state.Set(BoardState::STAGE_END_TURN);
		}
};

#endif
