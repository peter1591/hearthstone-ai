#include "FlowControl/FlowController-impl.h"

#include <iostream>
#include <chrono>

#include "Cards/PreIndexedCards.h"
#include "TestStateBuilder.h"
#include "UI/AIController.h"

void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
}

int main(void)
{
	Initialize();

	auto start_board_getter = []() -> state::State {
		return TestStateBuilder().GetState();
	};

	ui::AIController controller;
	while (true) {
		std::string cmd;
		std::cout << "Command: ";
		std::cin >> cmd;

		if (cmd == "h" || cmd == "help") {
			std::cout << "Commands: " << std::endl
				<< "h or help: show this message" << std::endl
				<< "start (secs): to run for a specified seconds" << std::endl
				<< "q or quit: quit" << std::endl;
		}
		else if (cmd == "start") {
			int secs = 0;
			std::cin >> secs;

			auto start = std::chrono::steady_clock::now();
			auto start_i = controller.GetStatistic().GetSuccededIterates();
			controller.Run(secs, start_board_getter);
			auto end_i = controller.GetStatistic().GetSuccededIterates();

			std::cout << std::endl;
			std::cout << "Done iterations: " << (end_i - start_i) << std::endl;
			std::cout << "====== Statistics =====" << std::endl;
			controller.GetStatistic().PrintMessage();

			auto now = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
			auto speed = (double)(end_i - start_i) / ms * 1000;
			std::cout << "Iterations per second: " << speed << std::endl;
			std::cout << std::endl;
		}
		else if (cmd == "q" || cmd == "quit") {
			std::cout << "Good bye!" << std::endl;
			break;
		}
	}

	return 0;
}
