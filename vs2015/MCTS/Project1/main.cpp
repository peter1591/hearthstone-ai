#include <time.h>
#include <random>
#include <map>
#include <iostream>

#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"
#include "game-ai/game-ai.h"

#include "game-engine/cards/card_FP1_007.h"

static void Run()
{
	constexpr int threads = 1;
	constexpr int sec_each_run = 1;
	constexpr int msec_total = 60 * 1000;

	std::mt19937 random_generator((unsigned int)time(nullptr));
	MCTS mcts[threads];
	std::vector<Task*> tasks;
	std::map<Task*, Task::PauseNotifier*> pause_notifiers;

	for (int i = 0; i < threads; ++i) {
		StartBoard start_board;
		mcts[i].Initialize(random_generator(), std::move(start_board));

		Task *new_task = new Task(mcts[i]);
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

		if (elapsed_ms > msec_total) break;

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

	for (const auto &task : tasks)
	{
		delete task;
	}
}

int main(void)
{
	std::cout << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	Run();

	std::cout << "Press Enter to exit...";
	std::cin.get();

	return 0;
}
