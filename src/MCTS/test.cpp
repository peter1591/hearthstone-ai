#include "FlowControl/FlowController-impl.h"

#include <iostream>

#include "TestStateBuilder.h"
#include "MCTS/MCTS.h"
#include "MCTS/MCTS-impl.h"

class ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return next_minion_put_location;
	}

	state::CardRef GetSpecifiedTarget(state::State & state, state::CardRef card_ref, std::vector<state::CardRef> const& targets)
	{
		assert(targets.size() == next_specified_target_count);

		if (next_specified_target_idx < 0) return state::CardRef();
		else return targets[next_specified_target_idx];
	}

	size_t ChooseOne(std::vector<Cards::CardId> const& cards) {
		choose_one_called = true;
		assert(next_choose_one_count == cards.size());
		return next_choose_one_idx;
	}

	int next_minion_put_location;

	int next_specified_target_count;
	int next_specified_target_idx;

	bool choose_one_called;
	int next_choose_one_count;
	int next_choose_one_idx;
};

class RandomGenerator : public state::IRandomGenerator
{
public:
	RandomGenerator() :called_times(0), next_rand(0) {}

	int Get(int exclusive_max)
	{
		++called_times;
		assert(next_rand < exclusive_max);
		return next_rand;
	}

	size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

	int Get(int min, int max)
	{
		++called_times;
		assert(next_rand >= min);
		assert(next_rand <= max);
		return min + next_rand;
	}

public:
	int called_times;
	int next_rand;
};

void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	std::cout << " Done." << std::endl;
}

int main(void)
{
	ActionParameterGetter action_parameter;
	RandomGenerator random;

	Initialize();

	mcts::MCTS mcts1;

	auto start_board_getter = [&]() {
		return TestStateBuilder().GetState();
	};

	for (int i = 0; i < 100000000; ++i) {
		if (i % 1 == 0) {
			std::cout << "Doing " << i << "th iteration." << std::endl;
		}
		mcts1.Iterate(start_board_getter);
	}

	return 0;
}