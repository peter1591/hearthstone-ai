#include "FlowControl/FlowController-impl.h"

#include <chrono>
#include <iostream>
#include <sstream>

#include "Cards/PreIndexedCards.h"
#include "MCTS/TestStateBuilder.h"
#include "UI/AIController.h"
#include "UI/CompetitionGuide.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
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
                mcts::policy::simulation::RandomPlayouts>);

	std::cout << "Parameters: " << std::endl;
	std::cout << "\tThreads: " << threads << std::endl;
	std::cout << "\tIterations: " << iterations << std::endl;
	std::cout << "\tSeed: " << seed << std::endl;

	ui::CompetitionGuide guide(rand);

	ui::AICompetitor first;
	ui::AICompetitor second;

	TestStateBuilder().GetState(rand());

	auto start_board_getter = [&]() -> state::State {
		return TestStateBuilder().GetStateWithRandomStartCard(rand());
	};

	guide.SetFirstCompetitor(&first);
	guide.SetSecondCompetitor(&second);

	guide.Start(start_board_getter, threads, iterations);

	return 0;
}
