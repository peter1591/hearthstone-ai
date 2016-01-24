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
	board.player_stat.crystal.Set(2, 2, 0, 0);

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystal.Set(2, 2, 0, 0);

	board.opponent_cards.Set(30);

	InitializeDeck1(deck_database, board.player_deck);
	InitializeHand1(deck_database, board.player_hand);
	
	GameEngine::Minion minion;
	minion.Set(111, 1, 1, 1);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(213, 3, 2, 3);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(333, 30, 3, 3);
	minion.TurnStart();
	board.opponent_minions.AddMinion(minion);

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
}


int main(void)
{
	MCTS mcts;
	MCTS::TraversedPathInfo traversed_path_info;

	GameEngine::Board board;

	InitializeBoard(board);
	board.DebugPrint();

	mcts.Initialize(board);

	int times = 0;

	time_t start = time(NULL);

	while (true) {
		GameEngine::Board board;
		traversed_path_info.Clear();

		TreeNode *node = mcts.SelectAndExpand(board, traversed_path_info);
		bool is_win = mcts.Simulate(board);
		mcts.BackPropagate(traversed_path_info, is_win);

		times++;

		if (times % 10000 == 0) {
			time_t now = time(NULL);
			int duration = now - start;
			std::cout << "About " << (double)times / duration << " rounds per second" << std::endl;
			mcts.DebugPrint();
			//std::cout << "Press any key to continue..." << std::endl;
			//std::cin.get();
		}
	}

	return 0;
}
