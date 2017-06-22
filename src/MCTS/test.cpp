#include "FlowControl/FlowController-impl.h"

#include <iostream>

#include "TestStateBuilder.h"
#include "MCTS/builder/TreeBuilder.h"
#include "MCTS/builder/TreeBuilder-impl.h"
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

	mcts::Statistic<> statistic;
	mcts::builder::TreeBuilder::TreeNode root_node;
	mcts::builder::TreeBuilder mcts1(statistic);
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

		mcts::Stage stage = mcts::kStageSelection;
		mcts::builder::TreeBuilder::TreeNode * node = &root_node;
		mcts::board::Board board = start_board_getter();
		updater.Clear();
		while (true)
		{
			mcts::builder::TreeBuilder::PerformResult result;

			if (stage == mcts::kStageSelection) result = mcts1.PerformSelect(node, board, &updater);
			else {
				assert(stage == mcts::kStageSimulation);
				result = mcts1.PerformSimulate(board);
			}

			assert(result.result != mcts::Result::kResultInvalid);

			if (result.result != mcts::Result::kResultNotDetermined) {
				bool credit = mcts::CreditPolicy::GetCredit(state::kPlayerFirst, result.result); // suppose AI is helping the first player
				updater.Update(credit);
				break;
			}

			// We use a flag here, since we cannot switch to simulation mode in sub-actions.
			if (stage == mcts::kStageSelection && result.new_node_created) {
				stage = mcts::kStageSimulation;
			}
			node = result.node;
		}
	}

	return 0;
}