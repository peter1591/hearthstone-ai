#include <iostream>
#include <functional>

#include "EventManager/HandlersManager.h"
#include "EventManager/Event.h"
#include "EventManager/StaticEvent.h"

static void test1()
{
	EventManager::HandlersManager mgr;

	std::function<void(EventManager::HandlersContainerController &, int)> callback1 =
		[](EventManager::HandlersContainerController & controller, int v) {
		std::cout << "callback1. v = " << v << std::endl;
	};
	std::function<void(EventManager::HandlersContainerController &, int)> callback2 =
		[](EventManager::HandlersContainerController & controller, int v) {
		std::cout << "callback2. v = " << v << std::endl;
	};
	std::function<void(EventManager::HandlersContainerController &, int)> callback3 =
		[](EventManager::HandlersContainerController & controller, int v) {
		std::cout << "callback3. v = " << v << std::endl;
		controller.Remove();
	};

	mgr.PushBack(EventManager::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(EventManager::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(EventManager::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(EventManager::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(EventManager::TriggerTypes::BeforeMinionSummoned(callback2));
	mgr.PushBack(EventManager::TriggerTypes::BeforeMinionSummoned(callback1));
	mgr.PushBack(EventManager::TriggerTypes::BeforeMinionSummoned(callback3));
	mgr.PushBack(EventManager::TriggerTypes::BeforeMinionSummoned(callback2));

	mgr.PushBack(5, EventManager::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(5, EventManager::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(5, EventManager::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(55, EventManager::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(55, EventManager::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(55, EventManager::TriggerTypes::AfterMinionSummoned(callback2));

	auto mgr2 = mgr;

	EventManager::Event<EventManager::TriggerTypes::AfterMinionSummoned> ev1(987);
	EventManager::Event<EventManager::TriggerTypes::BeforeMinionSummoned> ev2(98789);
	typedef EventManager::StaticEvent<EventManager::TriggerTypes::AfterMinionSummoned> EV1;
	typedef EventManager::StaticEvent<EventManager::TriggerTypes::BeforeMinionSummoned> EV2;

	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr, 777);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr, 7717);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr);

	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr2, 78877);
	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr, 89797);

	EventManager::CategorizedEvent<EventManager::TriggerTypes::AfterMinionSummoned> ev3(4, 199);
	EventManager::CategorizedEvent<EventManager::TriggerTypes::AfterMinionSummoned> ev4(55, 1999);
	typedef EventManager::StaticCategorizedEvent<EventManager::TriggerTypes::AfterMinionSummoned> EV3;

	std::cout << "--" << std::endl;
	EV3::TriggerEvent(mgr, 4, 999);
	std::cout << "--" << std::endl;
	ev3.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV3::TriggerEvent(mgr, 55, 12345);
	std::cout << "--" << std::endl;
	ev4.TriggerEvent(mgr);

	std::cout << "---" << std::endl;
	ev3.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	EV3::TriggerEvent(mgr2, 4, 999);
	std::cout << "---" << std::endl;
	ev4.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	EV3::TriggerEvent(mgr2, 55, 112592);
}

int main(void)
{
	test1();

	return 0;
}