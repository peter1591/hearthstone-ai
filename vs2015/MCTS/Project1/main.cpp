#include <time.h>
#include <random>
#include <map>
#include <iostream>

#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"
#include "task.h"
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
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion

	//hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
}

void InitializeBoard(GameEngine::Board &board)
{
	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystal.Set(1, 1, 0, 0);
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

	minion.Set(222, 30, 1, 2);
	minion.TurnStart();
	board.opponent_minions.AddMinion(minion);

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
}

static void Run()
{
	constexpr int threads = 4;
	constexpr int sec_each_run = 1;

	std::mt19937 random_generator((unsigned int)time(nullptr));
	MCTS mcts[threads];
	std::vector<Task*> tasks;
	std::map<Task*, Task::PauseNotifier*> pause_notifiers;

	GameEngine::Board board;
	InitializeBoard(board);
	board.DebugPrint();

	for (int i = 0; i < threads; ++i) {
		mcts[i].Initialize(random_generator(), board);
		Task *new_task = new Task(&mcts[i]);
		new_task->Initialize(std::thread(Task::ThreadCreatedCallback, new_task));
		tasks.push_back(new_task);

		pause_notifiers[new_task] = new Task::PauseNotifier;

		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		new_task->Start(run_until, pause_notifiers[new_task]);
	}

	auto start = std::chrono::steady_clock::now();
	for (int times = 0;; times++)
	{
		std::cout << "Waiting..." << std::endl;
		std::cout.flush();
		for (const auto &task : tasks)
		{
			pause_notifiers[task]->WaitUntilPaused();
		}

		std::cout << "Deciding..." << std::endl;
		std::cout.flush();
		Decider decider;
		for (int i = 0; i < threads; ++i) {
			decider.Add(mcts[i]);
		}
		auto best_moves = decider.GetBestMoves();
		best_moves.DebugPrint();

		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		int total_iterations = 0;
		for (auto const& task : tasks) total_iterations += task->GetIterationCount();
		std::cout << "Done " << total_iterations << " iterations in " << elapsed_ms << " ms"
			<< " (Average: " << ((double)total_iterations * 1000 / elapsed_ms) << " iterations per second.)" << std::endl;
		std::cout.flush();

		// start all threads for 10 second
		std::cout << "Resuming..." << std::endl;
		std::cout.flush();
		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		for (const auto &task : tasks)
		{
			task->Start(run_until, pause_notifiers[task]);
		}
	}

	for (const auto &task : tasks)
	{
		task->Stop();
	}
	for (const auto &task : tasks)
	{
		task->Join();
	}

	std::cout << "Threads stopped" << std::endl;
	std::cin.clear();
	std::cout << "Press Enter to exit...";
	std::cin.get();
}

int main(void)
{
	std::cout << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	Run();

	return 0;
}
