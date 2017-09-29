
#include <chrono>
#include <iostream>
#include <sstream>

#include "FlowControl/FlowController-impl.h"
#include "Cards/PreIndexedCards.h"
#include "MCTS/TestStateBuilder.h"
#include "UI/AIController.h"
#include "UI/CompetitionGuide.h"
#include "UI/InteractiveShell.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
}

void TestAI()
{
	srand(0);

	int tree_samples = 10000;
	std::mt19937 rand;

	auto start_board_getter = [](int seed) -> state::State {
		return TestStateBuilder().GetState(seed);
	};

	ui::AIController controller(tree_samples, rand);
	ui::InteractiveShell handler(&controller, start_board_getter);

	while (std::cin) {
		std::string cmdline;
		std::cout << "Command: ";
		std::getline(std::cin, cmdline);

		if (cmdline == "q" || cmdline == "quit") {
			std::cout << "Good bye!" << std::endl;
			break;
		}

		std::istringstream iss(cmdline);
		handler.DoCommand(iss, std::cout);
	}
}

int main(int argc, char *argv[])
{
	Initialize();
	TestAI();
	return 0;
}
