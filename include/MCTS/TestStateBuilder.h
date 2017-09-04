#pragma once

#include "state/State.h"

class TestStateBuilder
{
public:
	state::State GetStateWithRandomStartCard(int seed);
	state::State GetState(int seed);
};