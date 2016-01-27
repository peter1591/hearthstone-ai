#include <time.h>
#include <chrono>
#include "game-engine/deck-database.h"
#include "mcts.h"
#include "decider.h"

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
	minion.Set(111, 1, 7, 1);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(213, 3, 2, 3);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(333, 10, 3, 3);
	minion.TurnStart();
	board.opponent_minions.AddMinion(minion);

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
}

static void TestOperators()
{
	constexpr int threads = 2;
	MCTS mcts_debug;
	MCTS mcts[threads];
	unsigned int rand_seed = (unsigned int)time(NULL);
	GameEngine::Board board;

	InitializeBoard(board);

	for (int i = 0; i < threads; ++i) {
		mcts[i].Initialize(rand_seed, board);
	}
	mcts_debug.Initialize(rand_seed, board);

	for (int times = 0; ; ++times)
	{
		mcts[0].Iterate();
		mcts_debug.Iterate();

		if (mcts[0] != mcts_debug) // test comparison
		{
			throw std::runtime_error("check failed");
		}

		mcts[1] = mcts[0]; // test copy
		if (mcts[0] != mcts[1]) {
			throw std::runtime_error("check failed");
		}

		mcts[1].Iterate();
		mcts_debug.Iterate();
		if (mcts[1] != mcts_debug)
		{
			throw std::runtime_error("check failed");
		}

		mcts[0] = std::move(mcts[1]); // test move

		if (times % 1000 == 0) {
			Decider decider(std::move(mcts_debug));
			decider.DebugPrint();
		}
	}
}

static void TestMerge()
{
	constexpr int threads = 4;
	MCTS mcts[threads];
	GameEngine::Board board;

	InitializeBoard(board);

	for (int i = 0; i < threads; ++i) {
		mcts[i].Initialize((unsigned int)time(NULL), board);
	}

	auto start = std::chrono::steady_clock::now();
	for (int times = 0;; ++times)
	{
		MCTS mcts_copy;

		for (int i = 0; i < threads; ++i) {
			for (int j = 0; j < 1000; ++j) {
				mcts[i].Iterate();
			}
		}

		mcts_copy = mcts[0];
		Decider decider(std::move(mcts_copy));
		for (int i = 1; i < threads; ++i) {
			mcts_copy = mcts[i];
			decider.Merge(std::move(mcts_copy));
		}

		if (times % 1 == 0)
		{
			decider.DebugPrint();

			auto now = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
			auto avg = (double)elapsed_ms / times;
			std::cout << "Average: " << avg << " ms per iteration" << std::endl;
		}
	}
}

static void TestBasic()
{
	MCTS mcts;
	unsigned int rand_seed = (unsigned int)time(NULL);
	GameEngine::Board board;

	InitializeBoard(board);

	mcts.Initialize(rand_seed, board);

	for (int times = 0;; ++times)
	{
		mcts.Iterate();
		if (times % 10000 == 0) {
			Decider decider(std::move(mcts));
			decider.DebugPrint();
		}
	}
}

int main(void)
{
	//TestBasic();
	//TestOperators();
	TestMerge();
}