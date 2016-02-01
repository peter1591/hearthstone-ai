#ifndef STAGES_PLAYER_CHOOSE_BOARD_MOVE_H
#define STAGES_PLAYER_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>
#include <memory>

#include "stages/common.h"
#include "stages/weighted-moves.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"

namespace GameEngine {

class StagePlayerChooseBoardMove
{
	public:
		static const Stage stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;

		static void GetNextMoves(const Board &board, NextMoveGetter &next_move_getter)
		{
			std::vector<Move> next_moves;

			bool can_play_minion = !board.player_minions.IsFull();
			Move move;

			size_t guessed_next_moves = 0;

			// for play minions
			if (can_play_minion) {
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				guessed_next_moves += board.player_hand.GetCountByCardType(Card::TYPE_MINION) * (board.player_minions.GetMinions().size()+1);
#else
				guessed_next_moves += board.player_hand.GetCountByCardType(Card::TYPE_MINION);
#endif
			}

			// for attack
			guessed_next_moves += (board.player_minions.GetMinions().size()+1) * (board.opponent_minions.GetMinions().size()+1);

			next_moves.reserve(guessed_next_moves);

			// the choices to play a card from hand
			for (size_t hand_idx = 0; hand_idx < board.player_hand.GetCards().size(); ++hand_idx)
			{
				const Card &playing_card = board.player_hand.GetCards()[hand_idx];
				switch (playing_card.type) {
					case Card::TYPE_MINION:
						if (!can_play_minion) continue;
						if (board.player_stat.crystal.GetCurrent() < playing_card.cost) continue;
						StagePlayerChooseBoardMove::GetNextMoves_PlayMinion(hand_idx, board, next_moves);
					default:
						continue; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			for (int attacker_idx = -1; attacker_idx < (int)board.player_minions.GetMinions().size(); attacker_idx++) {
				if (attacker_idx == -1) {
					continue; // TODO: check if player has weapon
				} else {
					if (!board.player_minions.GetMinions()[attacker_idx].Attackable()) continue;
				}

				for (int attacked_idx = -1; attacked_idx < (int)board.opponent_minions.GetMinions().size(); attacked_idx++) {
					StagePlayerChooseBoardMove::GetNextMoves_Attack(attacker_idx, attacked_idx, next_moves);
				}
			}

			NextMoveGetter::ItemGetMoves *next_move_getter_item = new NextMoveGetter::ItemGetMoves();
			next_move_getter_item->moves.swap(next_moves);
			next_move_getter.items.push_back(next_move_getter_item);

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			next_move_getter.items.push_back(new NextMoveGetter::ItemGetMove(move));
		}

		static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
		{
			// heuristic goes here
			// 1. play minion is good (always put minion at the leftmost)
			// 2. minion attack is good
			// 3. hero attack is good
			// 4. effective spell is good

			thread_local static std::unique_ptr<WeightedMoves> ptr_moves = nullptr;
			if (!ptr_moves) ptr_moves.reset(new WeightedMoves());

			WeightedMoves & moves = *ptr_moves; // do not re-allocate at each call
			Move move;

			constexpr int weight_end_turn = 1;
			constexpr int weight_play_minion = 100;
			constexpr int weight_attack = 100;

			moves.Clear();

			// the choice to end turn
			move.action = Move::ACTION_END_TURN;
			moves.AddMove(move, weight_end_turn);

			// the choices to play a card from hand
			bool can_play_minion = !board.player_minions.IsFull();
			for (size_t hand_idx = 0; hand_idx < board.player_hand.GetCards().size(); ++hand_idx)
			{
				const Card &playing_card = board.player_hand.GetCards()[hand_idx];
				switch (playing_card.type) {
				case Card::TYPE_MINION:
					if (!can_play_minion) continue;
					if (board.player_stat.crystal.GetCurrent() < playing_card.cost) continue;
					move.action = Move::ACTION_PLAY_HAND_CARD_MINION;
					move.data.play_hand_card_minion_data.idx_hand_card = (int)hand_idx;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
					move.data.play_hand_card_minion_data.location = 0;
#endif
					moves.AddMove(move, weight_play_minion);
				default:
					continue; // TODO: handle other card types
				}
			}

			// the choices to attack by hero/minion
			for (int attacker_idx = -1; attacker_idx < (int)board.player_minions.GetMinions().size(); attacker_idx++) {
				if (attacker_idx == -1) {
					continue; // TODO: check if player has weapon
				}
				else {
					if (!board.player_minions.GetMinions()[attacker_idx].Attackable()) continue;
				}

				for (int attacked_idx = -1; attacked_idx < (int)board.opponent_minions.GetMinions().size(); attacked_idx++) {
					move.action = Move::ACTION_PLAYER_ATTACK;
					move.data.player_attack_data.attacker_idx = attacker_idx;
					move.data.player_attack_data.attacked_idx = attacked_idx;
					moves.AddMove(move, weight_attack);
				}
			}

			if (moves.Choose(rand, good_move) == false) {
				throw std::runtime_error("no action is available (should not happen)");
			}

			return;
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			switch (move.action)
			{
				case Move::ACTION_PLAY_HAND_CARD_MINION:
					return StagePlayerChooseBoardMove::PlayHandCardMinion(board, move);

				case Move::ACTION_PLAYER_ATTACK:
					return StagePlayerChooseBoardMove::PlayerAttack(board, move);

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
			move.data.play_hand_card_minion_data.idx_hand_card = (int)hand_card_idx;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			for (size_t i=0; i<=board.player_minions.GetMinions().size(); ++i) {
				move.data.play_hand_card_minion_data.location = i;
				next_moves.push_back(move);
			}
#else
			(void)board; // suppress warning
			next_moves.push_back(move);
#endif
		}

		static void PlayHandCardMinion(Board &board, const Move &move)
		{
			const Move::PlayHandCardMinionData &data = move.data.play_hand_card_minion_data;

			board.data.player_put_minion_data.idx_hand_card = data.idx_hand_card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.data.player_put_minion_data.location = data.location;
#endif

			board.stage = STAGE_PLAYER_PUT_MINION;
		}

		static void GetNextMoves_Attack(int attacker_idx, int attacked_idx, std::vector<Move> &next_moves)
		{
			Move move;

			move.action = Move::ACTION_PLAYER_ATTACK;
			move.data.player_attack_data.attacker_idx = attacker_idx;
			move.data.player_attack_data.attacked_idx = attacked_idx;

			next_moves.push_back(move);
		}

		static void PlayerAttack(Board &board, const Move &move)
		{
			const Move::PlayerAttackData &data = move.data.player_attack_data;

			board.data.player_attack_data.attacker_idx = data.attacker_idx;
			board.data.player_attack_data.attacked_idx = data.attacked_idx;

			board.stage = STAGE_PLAYER_ATTACK;
		}

		static void EndTurn(Board &board, const Move &)
		{
			board.stage = STAGE_PLAYER_TURN_END;
		}
};

} // namespace GameEngine

#endif
