#include <time.h>

#include <iostream>

#include "game-engine/board.h"
#include "game-engine/deck-database.h"

#include "mcts.h"

void InitializeDeck1(const GameEngine::DeckDatabase &deck_database, GameEngine::Deck &deck)
{
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
}

void InitializeHand1(const GameEngine::DeckDatabase &deck_database, GameEngine::Hand &hand)
{
	hand.AddCard(deck_database.GetCard(111));
	hand.AddCard(deck_database.GetCard(211));
	hand.AddCard(deck_database.GetCard(213));
	hand.AddCard(deck_database.GetCard(222));
	hand.AddCard(deck_database.GetCard(231));
}

void InitializeBoard(GameEngine::Board &board)
{
	GameEngine::DeckDatabase deck_database;

	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystal.Set(0, 0, 0, 0);

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystal.Set(0, 0, 0, 0);

	board.opponent_cards.Set(30);

	InitializeDeck1(deck_database, board.player_deck);
	InitializeHand1(deck_database, board.player_hand);
	
	GameEngine::Minion minion;
	minion.Set(111, 1, 1, 1);
	board.player_minions.AddMinion(minion);

	minion.Set(213, 2, 2, 3);
	board.player_minions.AddMinion(minion);

	minion.Set(333, 3, 3, 3);
	board.opponent_minions.AddMinion(minion);

	board.SetStateToPlayerTurnStart();
}


int main(void)
{
	MCTS mcts;

	GameEngine::Board board;

	InitializeBoard(board);
	board.DebugPrint();

	mcts.Initialize(board);

	int times = 0;

	while (true) {
		TreeNode *node = mcts.Select();

		if (node->stage_type != GameEngine::STAGE_TYPE_GAME_END) {
			GameEngine::Board board;
			TreeNode *new_node = mcts.Expand(node, board);
			bool is_win = mcts.Simulate(board);
			mcts.BackPropagate(new_node, is_win);

		} else {
			// if it's a game-end node, no need to expand & simulate
			// just back propagate again from the leaf node
			// TODO: check if this behavior is okay
			bool is_win = (node->stage == GameEngine::STAGE_WIN);

#ifdef DEBUG
			if (node->stage_type != GameEngine::STAGE_TYPE_GAME_END) {
				throw std::runtime_error("stage type is GAME_END, but it's not a win/loss");
			}
#endif

			mcts.BackPropagate(node, is_win);
		}

		times++;

		if (times % 10000 == 0) {
			mcts.DebugPrint();
			//std::cout << "Press any key to continue..." << std::endl;
			//std::cin.get();
		}
	}

	return 0;
}
