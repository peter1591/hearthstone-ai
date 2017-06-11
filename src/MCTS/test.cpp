#include "FlowControl/FlowController-impl.h"

#include <iostream>

#include "TestStateBuilder.h"
#include "MCTS/MCTS.h"
#include "MCTS/MCTS-impl.h"

void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	std::cout << " Done." << std::endl;
}

int main(void)
{
	Initialize();

	mcts::MCTS mcts1;

	auto start_board_getter = [&]() {
		return TestStateBuilder().GetState();
	};

	for (int i = 0; i < 100000000; ++i) {
		if (i % 10 == 0) {
			mcts1.PrintStatistic();

			if (i % 1000 == 0) {
				std::cout << "continue?";
				std::string dummy;
				std::getline(std::cin, dummy);
			}
		}
		mcts1.Iterate(start_board_getter);
	}

	return 0;
}