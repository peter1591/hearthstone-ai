#include "FlowControl/FlowController-impl.h"

#include <iostream>

#include "TestStateBuilder.h"
#include "MCTS/builder/TreeBuilder.h"
#include "MCTS/builder/TreeBuilder-impl.h"
#include "MCTS/MOMCTS.h"

void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	std::cout << " Done." << std::endl;
}

int main(void)
{
	Initialize();

	mcts::Statistic<> statistic;
	mcts::MOMCTS mcts2(statistic);

	auto start_board_getter = [&]() {
		return TestStateBuilder().GetState();
	};

	for (int i = 0; i < 100000000; ++i) {
		if (i % 10 == 0) {
			std::cout << "====== Statistics =====" << std::endl;
			std::cout << "Episodes: " << i << std::endl;
			statistic.PrintMessage();
		}

		if (i % 1000 == 999) {
			std::cout << "continue?";
			std::string dummy;
			std::getline(std::cin, dummy);
		}

		mcts2.Iterate(start_board_getter);
	}

	return 0;
}