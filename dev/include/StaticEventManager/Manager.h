#pragma once

#include "StaticEventManager/Triggerer.h"
#include "StaticEventManager/Events/GameStart.h"

namespace StaticEventManager
{
	class Manager
	{
	public:
		using GameStartEvent = StaticEventManager::Events::GameStartEvent;
	};
}