#include <iostream>

#include "EventManager/Manager.h"

int main(void)
{
	std::cout << "hello" << std::endl;

	EventManager::Manager manager;

	EventManager::Handlers::MinionSummoned handler1("a");
	EventManager::Handlers::MinionSummoned handler2("b");

	auto token1 = manager.PushBack(handler1);
	auto token2 = manager.PushBack(handler2);

	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	std::cout << "s1" << std::endl;
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	std::cout << "s2" << std::endl;
	manager.Remove(token1);
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	std::cout << "s3" << std::endl;
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	std::cout << "s4" << std::endl;
	auto token3 = manager.PushBack(handler1);
	auto token4 = manager.PushBack(handler2);
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	std::cout << "===" << std::endl;
	manager.Remove(token2);
	manager.TriggerEvent<EventManager::Handlers::MinionSummoned>();

	return 0;
}