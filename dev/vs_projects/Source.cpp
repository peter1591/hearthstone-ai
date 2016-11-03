#include <iostream>

#include "EventManager/Trigger.h"

int main(void)
{
	std::cout << "hello" << std::endl;

	EventManager::Trigger trigger;

	EventManager::Handlers::MinionSummoned handler1("a");
	EventManager::Handlers::MinionSummoned handler2("b");

	trigger.PushBack(handler1);
	trigger.PushBack(handler2);

	trigger.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	trigger.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	trigger.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	trigger.PushBack(EventManager::Handlers::MinionSummonedOnce("once-a"));
	trigger.PushBack(EventManager::Handlers::MinionSummonedOnce("once-b"));
	trigger.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	trigger.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<EventManager::Handlers::MinionSummoned>();
	trigger.TriggerEvent<EventManager::Handlers::MinionSummonedOnce>();

	std::cout << "===" << std::endl;
	trigger.PushBack(3, EventManager::Handlers::MinionSummoned("cat-3.1"));
	trigger.PushBack(3, EventManager::Handlers::MinionSummoned("cat-3.2"));
	trigger.PushBack(9999, EventManager::Handlers::MinionSummoned("cat-9999.1"));
	trigger.PushBack(9999, EventManager::Handlers::MinionSummoned("cat-9999.2"));
	trigger.PushBack(3, EventManager::Handlers::MinionSummonedOnce("cat-3.3"));
	trigger.PushBack(3, EventManager::Handlers::MinionSummonedOnce("cat-3.4"));
	trigger.PushBack(9999, EventManager::Handlers::MinionSummonedOnce("cat-9999.3"));
	trigger.PushBack(9999, EventManager::Handlers::MinionSummonedOnce("cat-9999.4"));

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(2);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(2);

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(9999);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(9999);

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(3);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(3);

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(2);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(2);

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(9999);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(9999);

	std::cout << "===" << std::endl;
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummoned>(3);
	trigger.TriggerEvent<int, EventManager::Handlers::MinionSummonedOnce>(3);

	return 0;
}