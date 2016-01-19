#ifndef _STAGES_PLAYER_CHOOSE_BOARD_MOVE_H
#define _STAGES_PLAYER_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StagePlayerChooseBoardMove
{
	public:
		static const bool is_random_node = false;
		static const bool is_player_turn = true;

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			bool can_play_minion = !board.player_minions.IsFull();
			Move move;

			size_t guessed_next_moves;

			guessed_next_moves = 1; // end turn
			if (can_play_minion) {
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				guessed_next_moves += board.player_hand.GetCountByCardType(Card::TYPE_MINION) * (board.player_minions.GetMinions().size()+1);
#else
				guessed_next_moves += board.player_hand.GetCountByCardType(Card::TYPE_MINION);
#endif
			}
			next_moves.reserve(guessed_next_moves);

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			next_moves.push_back(move);

			// the choices to play a card from hand
			for (size_t hand_idx = 0; hand_idx < board.player_hand.GetCards().size(); ++hand_idx)
			{
				const Card &playing_card = board.player_hand.GetCards()[hand_idx];
				switch (playing_card.type) {
					case Card::TYPE_MINION:
						if (!can_play_minion) continue;
						StagePlayerChooseBoardMove::GetNextMoves_PlayMinion(hand_idx, board, next_moves);
					default:
						continue; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			// TODO
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_PLAY_HAND_CARD_MINION:
					return StagePlayerChooseBoardMove::PlayHandCardMinion(board, move);
				case Move::ACTION_END_TURN:
					return StagePlayerChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void GetNextMoves_PlayMinion(size_t hand_card_idx, const Board &board, std::vector<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_PLAY_HAND_CARD_MINION;
			move.data.play_hand_card_minion_data.idx_hand_card = hand_card_idx;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (size_t i=0; i<=board.player_minions.GetMinions().size(); ++i) {
				move.data.play_hand_card_minion_data.location = i;
				next_moves.push_back(move);
			}
#else
			next_moves.push_back(move);
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			Minion minion;

			const Move::PlayHandCardMinionData &data = move.data.play_hand_card_minion_data;
			std::vector<Card>::const_iterator it_hand_card = board.player_hand.GetCards().begin() + data.idx_hand_card;

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

			board.stage = Board::STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = Board::STAGE_PLAYER_TURN_END;
		}
};

#endif
