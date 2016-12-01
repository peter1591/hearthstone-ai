#include <iostream>

#include "Cards/Database.h"
#include "FlowControl/FlowController.h"

class ActionParameterGetter : public FlowControl::IActionParameterGetter
{
public:
	int GetMinionPutLocation(int min, int max)
	{
		return min;
	}

	state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, std::vector<state::CardRef> const& targets)
	{
		return state::CardRef();
	}
};

class RandomGenerator : public FlowControl::IRandomGenerator
{
public:
	int Get(int exclusive_max)
	{
		return exclusive_max - 1;
	}

	int Get(int min, int max)
	{
		return max;
	}
};

void test2()
{
	std::cout << "Reading json file...";
	assert(Cards::Database::GetInstance().LoadJsonFile("../../include/Cards/cards.json"));
	std::cout << " Done." << std::endl;

	state::State state;
	ActionParameterGetter parameter_getter;
	RandomGenerator random;

	FlowControl::FlowController flow_controller(state, parameter_getter, random);
}