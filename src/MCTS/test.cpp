#include "FlowControl/FlowController-impl.h"

#include <iostream>

#include "TestStateBuilder.h"
#include "MCTS/MCTS.h"
#include "MCTS/MCTS-impl.h"
#include "MCTS/CreditPolicy.h"

void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	std::cout << " Done." << std::endl;
}

int main(void)
{
	Initialize();

	mcts::MCTS::TreeNode root_node;
	mcts::Statistic<> statistic;
	mcts::MCTS mcts1(root_node, statistic);
	mcts::MCTSUpdater updater;

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

		mcts::board::Board board = start_board_getter();
		mcts1.StartEpisode();
		updater.Clear();
		while (true)
		{
			mcts::Result result = mcts1.PerformOneAction(board, updater).second;
			assert(result != mcts::Result::kResultInvalid);

			if (result != mcts::Result::kResultNotDetermined) {
				bool credit = mcts::CreditPolicy::GetCredit(state::kPlayerFirst, result); // suppose AI is helping the first player
				updater.Update(credit);
				break;
			}
		}
	}

	return 0;
}