#include "FlowControl/FlowController-impl.h"

#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

#include "Cards/PreIndexedCards.h"
#include "MCTS/TestStateBuilder.h"
#include "judge/Judger.h"
#include "agents/MCTSAgent.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;

  unsigned int rand_seed = std::random_device()();
  srand(rand_seed);
  std::cout << "Initialize with random seed: " << rand_seed << std::endl;
}

int main(int argc, char *argv[])
{
	Initialize();

	if (argc != 3) {
		std::cout << "Usage: "
			<< argv[0]
			<< " (threads)"
			<< " (iterations)"
			<< std::endl;
		return 0;
	}

	int threads = 1;
	uint64_t iterations = 1000;

	auto seed = std::random_device()();
	std::mt19937 rand(seed);

	{
		std::istringstream ss(argv[1]);
		ss >> threads;
	}
	{
		std::istringstream ss(argv[2]);
		ss >> iterations;
	}

	static_assert(std::is_same_v<
								mcts::StaticConfigs::SimulationPhaseSelectActionPolicy,
								mcts::policy::simulation::RandomPlayouts> ||
								std::is_same_v<
								mcts::StaticConfigs::SimulationPhaseSelectActionPolicy,
								mcts::policy::simulation::RandomPlayoutWithHardCodedRules>);

	std::cout << "Parameters: " << std::endl;
	std::cout << "\tThreads: " << threads << std::endl;
	std::cout << "\tIterations: " << iterations << std::endl;
	std::cout << "\tSeed: " << seed << std::endl;

	judge::Judger judger(rand);
	constexpr int root_samples = 10;

	agents::MCTSAgent first(threads, root_samples);
	agents::MCTSAgent second(threads, root_samples);

	auto start_board_getter = [&]() -> state::State {
		return TestStateBuilder().GetStateWithRandomStartCard(rand());
	};

	judger.SetFirstAgent(&first);
	judger.SetSecondAgent(&second);

	auto last_show = std::chrono::steady_clock::now();
	judger.Start(start_board_getter, rand(), [&](state::State const& state) {
		std::cout << "Turn: " << state.GetTurn() << std::endl;
	}, [iterations, last_show](uint64_t	now_iterations) mutable {
		if (now_iterations >= iterations) {
			std::cout << "Total iterations: " << now_iterations << std::endl;
			return false;
		}

		auto now = std::chrono::steady_clock::now();
		auto after_last_shown = std::chrono::duration_cast<std::chrono::seconds>(now - last_show).count();
		if (after_last_shown > 5) {
			double percent = (double)now_iterations / iterations;
			std::cout << "Iterations: " << now_iterations << " (" << percent * 100.0 << "%)" << std::endl;
			last_show = now;
		}
		return true;
	});

	return 0;
}
