#include <iostream>

#include "EventManager/Manager.h"

int main(void)
{
	std::cout << "hello" << std::endl;

	EventManager::Manager manager;

	EventManager::Handlers::MinionSummoned handler1("a");
	EventManager::Handlers::MinionSummoned handler2("b");

	manager.PushBack(handler1);
	manager.PushBack(handler2);

	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	manager.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	manager.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	manager.PushBack(EventManager::Handlers::MinionSummonedOnce("once-a"));
	manager.PushBack(EventManager::Handlers::MinionSummonedOnce("once-b"));
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	manager.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	manager.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	manager.PushBack(3, EventManager::Handlers::MinionSummoned("cat-3.1"));
	manager.PushBack(3, EventManager::Handlers::MinionSummoned("cat-3.2"));
	manager.PushBack(9999, EventManager::Handlers::MinionSummoned("cat-9999.1"));
	manager.PushBack(9999, EventManager::Handlers::MinionSummoned("cat-9999.2"));
	manager.PushBack(3, EventManager::Handlers::MinionSummonedOnce("cat-3.3"));
	manager.PushBack(3, EventManager::Handlers::MinionSummonedOnce("cat-3.4"));
	manager.PushBack(9999, EventManager::Handlers::MinionSummonedOnce("cat-9999.3"));
	manager.PushBack(9999, EventManager::Handlers::MinionSummonedOnce("cat-9999.4"));

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(2);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(2);

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(9999);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(9999);

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(3);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(3);

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(2);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(2);

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(9999);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(9999);

	std::cout << "===" << std::endl;
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(3);
	manager.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(3);

	return 0;
}