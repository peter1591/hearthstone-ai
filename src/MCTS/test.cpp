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
			std::cout << "====== Statistics =====" << std::endl;
			std::cout << "Episodes: " << i << std::endl;
			mcts1.PrintStatistic();
		}

		if (i % 1000 == 999) {
			std::cout << "continue?";
			std::string dummy;
			std::getline(std::cin, dummy);
		}

		mcts1.StartEpisode(start_board_getter);

		while (true)
		{
			mcts::Result result = mcts1.PerformOneAction();
			assert(result != mcts::Result::kResultInvalid);

			if (result != mcts::Result::kResultNotDetermined) {
				mcts1.EpisodeFinished(result);
				break;
			}
		}
	}

	return 0;
}