#pragma once

#include "state/State.h"

class TestStateBuilder
{
public:
	state::State GetStateWithRandomStartCard(int start_card_seed, int state_seed);
	state::State GetState(int seed);
};
