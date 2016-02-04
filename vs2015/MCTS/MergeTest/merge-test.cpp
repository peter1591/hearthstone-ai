#include <time.h>

#include <sstream>
#include <chrono>
#include <vector>

#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"
#include "mcts.h"
#include "decider.h"

void InitializeDeck1(GameEngine::Deck &deck)
{
	for (int i = 0; i < 27; ++i) {
		deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	}
}

void InitializeHand1(GameEngine::Hand &hand)
{
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_AT_087)); // 321 [CHARGE] [SHIELD]
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_522)); // 321 [POISON] [STEALTH]
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_065)); // 344 [FORGETFUL]
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion

	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
}

void InitializeBoard(GameEngine::Board &board)
{
	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystal.Set(3, 3, 0, 0);
	board.player_stat.fatigue_damage = 0;

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystal.Set(0, 0, 0, 0);
	board.opponent_stat.fatigue_damage = 0;

	board.opponent_cards.Set(4, 26);

	InitializeDeck1(board.player_deck);
	InitializeHand1(board.player_hand);

	GameEngine::Minion minion;
	//minion.Set(111, 1, 1, 1);
	//minion.TurnStart();
	//board.player_minions.AddMinion(minion);

	//minion.Set(213, 2, 2, 3);
	//minion.TurnStart();
	//board.player_minions.AddMinion(minion);

	minion.Set(222, 10, 10, 2);
	minion.TurnStart();
	minion.SetTaunt(true);
	minion.SetStealth(true);
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

static void InteractiveTest()
{
	MCTS mcts;
	//unsigned int rand_seed = (unsigned int)time(NULL);
	unsigned int rand_seed = 0;
	GameEngine::Board board;

	InitializeBoard(board);

	srand((unsigned int)time(nullptr));

	while (true)
	{
		board.DebugPrint();

		if (board.GetStageType() == GameEngine::STAGE_TYPE_GAME_END)
		{
			std::cout << "Game ends!!!!!" << std::endl;
			break;
		}

		GameEngine::Move next_move;
		if (board.GetStageType() == GameEngine::STAGE_TYPE_GAME_FLOW)
		{
			unsigned int rand_seed = (unsigned int) rand();
			std::cout << "Choose random for game flow move: (default: " << rand_seed << "): ";
			std::string str_rand_seed;
			std::getline(std::cin, str_rand_seed);
			if (!str_rand_seed.empty()) {
				std::stringstream ss;
				ss << str_rand_seed;
				ss >> rand_seed;
			}
			next_move = GameEngine::Move::GetGameFlowMove(rand_seed);
		}
		else
		{
			std::vector<GameEngine::Move> next_moves;

			GameEngine::NextMoveGetter next_move_getter;
			board.GetNextMoves(next_move_getter);
			while (!next_move_getter.Empty()) {
				GameEngine::Move move;
				next_move_getter.GetNextMove(move);
				next_moves.push_back(move);
			}

			while (true) {
				int idx = 0;
				std::cout << "Next moves: " << std::endl;
				for (auto const& next_move_candidate : next_moves)
				{
					std::cout << "\t" << idx << ". " << next_move_candidate.GetDebugString() << std::endl;
					idx++;
				}

				std::cout << "Please choose next move (default: 0): ";
				int next_move_idx = 0;

				std::string str_next_move_idx;
				std::getline(std::cin, str_next_move_idx);
				if (str_next_move_idx.empty())
				{
					if (std::cin.eof()) break;
				}
				else {
					std::stringstream ss;
					ss << str_next_move_idx;
					ss >> next_move_idx;

					if (next_move_idx < 0 || next_move_idx >= next_moves.size()) {
						std::cout << "Please enter a correct number (Ctrl+Z to exit)" << std::endl;
						continue;
					}
				}

				next_move = next_moves[next_move_idx];
				break;
			}
		}

		if (std::cin.eof()) break;

		bool is_random;
		board.ApplyMove(next_move, &is_random);
		if (is_random) std::cout << "!!! This move is random!!!" << std::endl;
		else std::cout << "!!! This move is non-random!!!" << std::endl;
	}
}

int main(void)
{
	std::cout << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	//TestBasic();
	//TestOperators();

	InteractiveTest();
}