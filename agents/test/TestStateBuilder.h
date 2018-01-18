#pragma once

#include <random>

#include "state/State.h"

class TestStateBuilder
{
public:
	state::State GetStateWithRandomStartCard(int start_card_seed, std::mt19937 & random);
	state::State GetState(std::mt19937 & random);
};
