#include <iostream>

#include "EventManager/HandlersManager.h"
#include "EventManager/Event.h"

int main(void)
{
	std::cout << "hello" << std::endl;

	EventManager::HandlersManager mgr;

	EventManager::Handlers::MinionSummoned handler1("a");
	EventManager::Handlers::MinionSummoned handler2("b");
	EventManager::Handlers::MinionSummonedOnce handler3("once-c");

	mgr.PushBack(handler1);
	mgr.PushBack(handler2);
	mgr.PushBack(handler3);

	mgr.PushBack(5, handler1);
	mgr.PushBack(5, handler2);
	mgr.PushBack(55, handler1);
	mgr.PushBack(55, handler2);
	mgr.PushBack(55, handler3);

	std::tuple<int> a;

	EventManager::Event<EventManager::Handlers::MinionSummoned> ev1(987, "h199", 3.15);
	EventManager::Event<EventManager::Handlers::MinionSummonedOnce> ev2;
	ev1.TriggerEvent(mgr);
	ev2.TriggerEvent(mgr);
	ev2.TriggerEvent(mgr);

	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev3(4, 199, "kkk", 9.64);
	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev4(55, 199, "kkk", 123.656);
	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummonedOnce> ev5(55);
	ev3.TriggerEvent(mgr);
	ev4.TriggerEvent(mgr);
	ev5.TriggerEvent(mgr);
	ev5.TriggerEvent(mgr);
	
	mgr.PushBack(55, handler3);
	ev5.TriggerEvent(mgr);

	std::cout << "==" << std::endl;

	return 0;
}