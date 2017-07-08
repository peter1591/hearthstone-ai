#include "FlowControl/FlowController-impl.h"

#include <iostream>
#include <chrono>

#include "TestStateBuilder.h"
#include "MCTS/MOMCTS.h"
#include "MCTS/builder/TreeBuilder.h"
#include "MCTS/builder/TreeBuilder-impl.h"

#ifdef PROFILING
extern "C" {
extern void __gcov_flush();
}
#endif

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
	mcts::MOMCTS mcts(statistic);

	auto start_board_getter = [&]() {
		return TestStateBuilder().GetState();
	};

	int start_i = 0;
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < 10000000; ++i) {
		if (i % 1000 == 0) {
			std::cout << "====== Statistics =====" << std::endl;
			std::cout << "Episodes: " << i << std::endl;
			statistic.PrintMessage();
			
			auto now = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
			auto speed = (double)(i-start_i) / ms * 1000;
			std::cout << "Iterations per second: " << speed << std::endl;

			start_i = i + 1;
			start = std::chrono::steady_clock::now();

#ifdef PROFILING
			std::cout << "Flushing gconv profiling data...";
			__gcov_flush();
			std::cout << "done!" << std::endl;
#endif
		}

		if (i % 1000000 == 999999) {
			std::cout << "continue?";
			std::string dummy;
			std::getline(std::cin, dummy);
		}

		mcts.Iterate(start_board_getter);
	}

	return 0;
}
