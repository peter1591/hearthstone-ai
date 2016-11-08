#include <iostream>

#include "EventManager/HandlersManager.h"
#include "EventManager/Event.h"
#include "EventManager/StaticEvent.h"

static void test1()
{
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

	auto mgr2 = mgr;

	EventManager::Event<EventManager::Handlers::MinionSummoned> ev1(987, "h199", 3.15);
	EventManager::Event<EventManager::Handlers::MinionSummonedOnce> ev2;
	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr2);

	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev3(4, 199, "kkk", 9.64);
	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummoned> ev4(55, 199, "kkk", 123.656);
	EventManager::CategorizedEvent<EventManager::Handlers::MinionSummonedOnce> ev5(55);
	std::cout << "--" << std::endl;
	ev3.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev4.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev5.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	ev5.TriggerEvent(mgr);
	std::cout << "---" << std::endl;
	ev3.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	ev4.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	ev5.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	ev5.TriggerEvent(mgr2);
	
	std::cout << "--" << std::endl;
	mgr.PushBack(55, handler3);
	ev5.TriggerEvent(mgr);
	std::cout << "---" << std::endl;
	ev5.TriggerEvent(mgr2);

	std::cout << "==" << std::endl;
}

static void test2()
{
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

	auto mgr2 = mgr;

	typedef EventManager::StaticEvent<EventManager::Handlers::MinionSummoned> EV1;
	typedef EventManager::StaticEvent<EventManager::Handlers::MinionSummonedOnce> EV2;
	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr, 987, "h199", 3.15);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr2, 987, "h199", 3.15);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr2);

	typedef EventManager::StaticCategorizedEvent<EventManager::Handlers::MinionSummoned> EV3;
	typedef EventManager::StaticCategorizedEvent<EventManager::Handlers::MinionSummoned> EV4;
	typedef EventManager::StaticCategorizedEvent<EventManager::Handlers::MinionSummonedOnce> EV5;
	std::cout << "--" << std::endl;
	EV3::TriggerEvent(mgr, 4, 199, "kkk", 9.64);
	std::cout << "--" << std::endl;
	EV4::TriggerEvent(mgr, 55, 199, "kkk", 123.656);
	std::cout << "--" << std::endl;
	EV5::TriggerEvent(mgr, 55);
	std::cout << "--" << std::endl;
	EV5::TriggerEvent(mgr, 55);
	std::cout << "---" << std::endl;
	EV3::TriggerEvent(mgr2, 4, 199, "kkk", 9.64);
	std::cout << "---" << std::endl;
	EV4::TriggerEvent(mgr2, 55, 199, "kkk", 123.656);
	std::cout << "---" << std::endl;
	EV5::TriggerEvent(mgr2, 55);
	std::cout << "---" << std::endl;
	EV5::TriggerEvent(mgr2, 55);

	std::cout << "--" << std::endl;
	mgr.PushBack(55, handler3);
	EV5::TriggerEvent(mgr, 55);
	std::cout << "---" << std::endl;
	EV5::TriggerEvent(mgr2, 55);

	std::cout << "==" << std::endl;
}

int main(void)
{
	//test1();
	test2();

	return 0;
}