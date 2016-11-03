#include <iostream>

#include "EventManager/Trigger.h"
#include "EventManager/Event.h"

int main(void)
{
	std::cout << "hello" << std::endl;

	EventManager::Trigger trigger;

	EventManager::Handlers::MinionSummoned handler1("a");
	EventManager::Handlers::MinionSummoned handler2("b");
	EventManager::Handlers::MinionSummonedOnce handler3("once-c");

	trigger.PushBack(handler1);
	trigger.PushBack(handler2);
	trigger.PushBack(handler3);

	trigger.PushBack(5, handler1);
	trigger.PushBack(5, handler2);
	trigger.PushBack(55, handler1);
	trigger.PushBack(55, handler2);

	std::tuple<int> a;

	EventManager::Event<EventManager::Handlers::MinionSummoned> ev1(987, "h199", 3.15);
	EventManager::Event<EventManager::Handlers::MinionSummonedOnce> ev2;
	ev1.TriggerEvent(trigger);
	ev2.TriggerEvent(trigger);

	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev3(4, 199, "kkk", 9.64);
	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev4(55, 199, "kkk", 123.656);
	ev3.TriggerEvent(trigger);
	ev4.TriggerEvent(trigger);

	return 0;
}