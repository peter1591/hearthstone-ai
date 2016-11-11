#pragma once

#include "State/Players.h"
#include "EntitiesManager/EntitiesManager.h"

namespace State
{
	class State
	{
	public:
		Players players;
		EntitiesManager mgr;
	};
}
