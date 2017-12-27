#include "engine/Game-impl.h"

#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

#include "Cards/PreIndexedCards.h"
#include "TestStateBuilder.h"
#include "judge/Judger.h"
#include "agents/MCTSAgent.h"

static void Initialize(unsigned int rand_seed)
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;

	srand(rand_seed);
}

class IterationCallback
{
public:
	IterationCallback() : first_time_(true), max_iterations_(0), last_shown_(){}

	void Initialize(uint64_t max_iterations) {
		first_time_ = true;
		max_iterations_ = max_iterations;
		last_shown_ = std::chrono::steady_clock::now();
	}

	bool operator()(engine::view::BoardRefView board_view, uint64_t iteration) {
		if (first_time_) {
			std::cout << "Turn: " << board_view.GetTurn() << std::endl;
			first_time_ = false;
		}

		if (iteration >= max_iterations_) {
			std::cout << "Total iterations: " << iteration << std::endl;
			first_time_ = true;
			return false;
		}

		auto now = std::chrono::steady_clock::now();
		auto after_last_shown = std::chrono::duration_cast<std::chrono::seconds>(now - last_shown_).count();
		if (after_last_shown > 5) {
			double percent = (double)iteration / max_iterations_;
			std::cout << "Iterations: " << iteration << " (" << percent * 100.0 << "%)" << std::endl;
			last_shown_ = now;
		}
		return true;
	}

private:
	bool first_time_;
	uint64_t max_iterations_;
	std::chrono::steady_clock::time_point last_shown_;
};

int main(int argc, char *argv[])
{
	auto seed = std::random_device()();

	std::cout << "Initialize with random seed: " << seed << std::endl;
	std::mt19937 rand(seed);

	Initialize(rand());

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

	using MCTSAgent = agents::MCTSAgent<IterationCallback>;

	judge::Judger<MCTSAgent> judger(rand);
	constexpr int root_samples = 10;

	IterationCallback iteration_cb;
	MCTSAgent first(threads, root_samples);
	MCTSAgent second(threads, root_samples);

	first.SetupIterationCallback(iterations);
	second.SetupIterationCallback(iterations);

	auto start_board_seed = rand();
	auto start_board_getter = [&](int rnd) -> state::State {
		return TestStateBuilder().GetStateWithRandomStartCard(start_board_seed, rnd);
	};

	judger.SetFirstAgent(&first);
	judger.SetSecondAgent(&second);

	judger.Start(start_board_getter, rand());

	return 0;
}
