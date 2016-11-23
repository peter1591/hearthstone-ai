#pragma once

#include "StaticEventManager/Triggerer.h"
#include "StaticEventManager/Events/GameStart.h"
#include "StaticEventManager/Events/RemovedFromZone.h"

namespace StaticEventManager
{
	class Manager
	{
	public:
		using GameStartEvent = StaticEventManager::Events::GameStartEvent;

		template <Entity::CardType RemovingCardType, Entity::CardZone RemovingCardZone>
		using RemovedFromZone = StaticEventManager::Events::RemovedFromZoneEvent<RemovingCardType, RemovingCardZone>;
	};
}