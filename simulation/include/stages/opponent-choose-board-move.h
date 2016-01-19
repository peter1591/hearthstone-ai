#ifndef STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H
#define STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H

#include <iostream> // TODO: debug only
#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageOpponentChooseBoardMove
{
	public:
		static const Stage stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		static const bool is_random_node = false;
		static const bool is_player_turn = false;
		static std::string GetStageStringName() { return "StageOpponentChooseBoardMove"; }

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			size_t guessed_next_moves;
			bool can_play_minion = !board.opponent_minions.IsFull();
			Move move;
			std::vector<Card> playable_minions;

			guessed_next_moves = 1; // end turn
			if (can_play_minion) {
				board.opponent_cards.GetPossiblePlayableMinions(board.opponent_stat, playable_minions);
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				guessed_next_moves += playable_minions.size() * (board.opponent_minions.GetMinions().size()+1);
#else
				guessed_next_moves += playable_minions.size();
#endif
			}
			next_moves.reserve(guessed_next_moves);

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			next_moves.push_back(move);

			// the choices to play a card from hand
			if (can_play_minion) {
				for (const auto &card : playable_minions) {
					StageOpponentChooseBoardMove::GetNextMoves_PlayMinion(card, board, next_moves);
				}
			}

			// the choices to attack by hero/minion
			// TODO
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_OPPONENT_PLAY_MINION:
					return StageOpponentChooseBoardMove::PlayHandCardMinion(board, move);
				case Move::ACTION_END_TURN:
					return StageOpponentChooseBoardMove::EndTurn(board, move);

				default:
					throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
			}
		}

	private:
		static void GetNextMoves_PlayMinion(const Card &card, const Board &board, std::vector<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_OPPONENT_PLAY_MINION;
			move.data.opponent_play_minion_data.card = card;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (size_t i=0; i<=board.opponent_minions.GetMinions().size(); ++i) {
				move.data.opponent_play_minion_data.location = i;
				next_moves.push_back(move);
			}
#else
			next_moves.push_back(move);
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			Minion minion;

			const Move::OpponentPlayMinionData &data = move.data.opponent_play_minion_data;

			board.opponent_stat.crystal.CostCrystals(data.card.cost);

			// TODO: handle battlecry
			minion.card_id = data.card.id;
			minion.max_hp = data.card.data.minion.hp;
			minion.hp = minion.max_hp;
			minion.attack = data.card.data.minion.attack;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.opponent_minions.AddMinion(minion, data.location);
#else
			board.opponent_minions.AddMinion(minion); // add to the rightmost
#endif

			board.opponent_cards.PlayCardFromHand(data.card);
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = STAGE_OPPONENT_TURN_END;
		}
};

#endif
