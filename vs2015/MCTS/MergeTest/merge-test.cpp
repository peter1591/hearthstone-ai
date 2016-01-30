#include <time.h>
#include <chrono>
#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"
#include "mcts.h"
#include "decider.h"

void InitializeDeck1(const GameEngine::CardDatabase &card_database, GameEngine::Deck &deck)
{
	for (int i = 0; i < 30; ++i) {
		deck.AddCard(card_database.GetCard(CARD_ID_CS2_120));
	}
}

void InitializeHand1(const GameEngine::CardDatabase &card_database, GameEngine::Hand &hand)
{
	hand.AddCard(card_database.GetCard(CARD_ID_GVG_092t));
	hand.AddCard(card_database.GetCard(CARD_ID_CS2_120));
	hand.AddCard(card_database.GetCard(CARD_ID_CS2_120));
	hand.AddCard(card_database.GetCard(CARD_ID_CS2_120));
}

void InitializeBoard(GameEngine::Board &board)
{
	GameEngine::CardDatabase card_database;

	std::cout << "Loading card database..." << std::endl;
	if (!card_database.ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystal.Set(1, 1, 0, 0);
	board.player_stat.fatigue_damage = 0;

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystal.Set(0, 0, 0, 0);
	board.opponent_stat.fatigue_damage = 0;

	board.opponent_cards.Set(4, 26);

	InitializeDeck1(card_database, board.player_deck);
	InitializeHand1(card_database, board.player_hand);

	GameEngine::Minion minion;
	//minion.Set(111, 1, 1, 1);
	//minion.TurnStart();
	//board.player_minions.AddMinion(minion);

	//minion.Set(213, 2, 2, 3);
	//minion.TurnStart();
	//board.player_minions.AddMinion(minion);

	//minion.Set(222, 2, 2, 2);
	//minion.TurnStart();
	//board.opponent_minions.AddMinion(minion);

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
			Decider decider;
			decider.Add(mcts_debug);
			decider.DebugPrint();
		}
	}
}

static void TestBasic()
{
	MCTS mcts;
	//unsigned int rand_seed = (unsigned int)time(NULL);
	unsigned int rand_seed = 0;
	GameEngine::Board board;

	InitializeBoard(board);

	mcts.Initialize(rand_seed, board);

	for (int times = 0;; ++times)
	{
		mcts.Iterate();
		if (times % 10000 == 0) {
			Decider decider;
			decider.Add(mcts);
			decider.DebugPrint();
			//std::cout << "Press Enter to continue...";
			//std::cin.get();
		}
	}
}

int main(void)
{
	//TestBasic();
	TestOperators();
}