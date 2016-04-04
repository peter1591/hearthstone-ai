#include <time.h>

#include <sstream>
#include <chrono>
#include <vector>

#include "game-ai/game-ai.h"

static void TestBasic()
{
	MCTS mcts;
	//unsigned int rand_seed = (unsigned int)time(NULL);
	unsigned int rand_seed = 0;
	StartBoard start_board;

	srand(rand_seed);
	mcts.Initialize(rand_seed, std::move(start_board));

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
	StartBoard start_board;

	unsigned int main_rand_seed = (unsigned int)time(nullptr);

	main_rand_seed = 0;
	srand(main_rand_seed);
	GameEngine::Board board = start_board.GetBoard(main_rand_seed);

	while (true)
	{
		if (board.GetStageType() == GameEngine::STAGE_TYPE_GAME_END)
		{
			std::cout << "Game ends!!!!!" << std::endl;
			break;
		}

		board.DebugPrint();

		GameEngine::Move next_move;
		if (board.GetStageType() == GameEngine::STAGE_TYPE_GAME_FLOW)
		{
			bool is_deterministic;
			board.GetNextMoves(next_move, is_deterministic);
		}
		else
		{
			bool is_deterministic;
			std::vector<GameEngine::Move> next_moves;

			GameEngine::NextMoveGetter next_move_getter;
			board.GetNextMoves(next_move_getter, is_deterministic);
			GameEngine::Move next_move_getter_result;
			while (next_move_getter.GetNextMove(board, next_move_getter_result)) {
				next_moves.push_back(next_move_getter_result);
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

class A
{
public:
	virtual bool EqualsTo(A const& rhs) const = 0;
};

class B : public A
{
public:
	bool EqualsTo(A const& rhs) const { return dynamic_cast<decltype(this)>(&rhs) != nullptr; }
};

class C : public A
{
public:
	bool EqualsTo(A const& rhs) const { return dynamic_cast<decltype(this)>(&rhs) != nullptr; }
};

static void TestClassEquality()
{
	B b1, b2;
	C c1, c2;

	if (!b1.EqualsTo(b2)) std::cout << "failed" << std::endl;
	if (!c1.EqualsTo(c2)) std::cout << "failed" << std::endl;
	if (b1.EqualsTo(c1)) std::cout << "failed" << std::endl;
	if (b1.EqualsTo(c2)) std::cout << "failed" << std::endl;
	if (c1.EqualsTo(b1)) std::cout << "failed" << std::endl;
	if (c1.EqualsTo(b2)) std::cout << "failed" << std::endl;

	std::cout << "ok" << std::endl;
}

int main(void)
{
	std::cout << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	//TestBasic();
	InteractiveTest();
	//TestClassEquality();
}