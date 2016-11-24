#include <iostream>
#include <functional>

#include "State/Events/HandlersManager.h"
#include "State/Events/Event.h"
#include "State/Events/StaticEvent.h"
#include "State/Events/StaticEvents.h"
#include "FlowControl/Context/BeforeMinionSummoned.h"
#include "State/State.h"
#include "State/Cards/Card.h"

using namespace State;

static void test1()
{
	Events::HandlersManager mgr;

	std::function<void(Events::HandlersContainerController &)> callback1 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback1" << std::endl;
	};
	std::function<void(Events::HandlersContainerController &)> callback2 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback2" << std::endl;
	};
	std::function<void(Events::HandlersContainerController &)> callback3 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback1_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback1" << std::endl;
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback2_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback2" << std::endl;
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback3_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};

	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback2_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback1_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback3_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback2_2));

	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback2));

	auto mgr2 = mgr;

	::State::State state;
	CardRef card_ref;
	Cards::RawCard raw_card;
	Cards::Card card(raw_card);
	FlowControl::Context::BeforeMinionSummoned context1(state, card_ref, card);

	Events::Event<Events::TriggerTypes::AfterMinionSummoned> ev1;
	Events::Event<Events::TriggerTypes::BeforeMinionSummoned> ev2(context1);
	typedef Events::StaticEvent<Events::TriggerTypes::AfterMinionSummoned> EV1;
	typedef Events::StaticEvent<Events::TriggerTypes::BeforeMinionSummoned> EV2;

	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr, context1);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr);

	std::cout << "--" << std::endl;
	EV1::TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	EV2::TriggerEvent(mgr2, context1);

	Events::CategorizedEvent<Events::TriggerTypes::AfterMinionSummoned> ev3(4);
	Events::CategorizedEvent<Events::TriggerTypes::AfterMinionSummoned> ev4(55);
	typedef Events::StaticCategorizedEvent<Events::TriggerTypes::AfterMinionSummoned> EV3;

	std::cout << "--" << std::endl;
	EV3::TriggerEvent(mgr, 4);
	std::cout << "--" << std::endl;
	ev3.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	EV3::TriggerEvent(mgr, 55);
	std::cout << "--" << std::endl;
	ev4.TriggerEvent(mgr);

	std::cout << "---" << std::endl;
	ev3.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	EV3::TriggerEvent(mgr2, 4);
	std::cout << "---" << std::endl;
	ev4.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	EV3::TriggerEvent(mgr2, 55);
}

static void test2()
{
	typedef Events::StaticEvent<Events::TriggerTypes::BeforeMinionSummoned> EV1;
	typedef Events::StaticEvent<Events::TriggerTypes::BeforeMinionSummoned> EV2;

	Events::HandlersManager mgr;

	std::function<void(Events::HandlersContainerController &)> callback1 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback1" << std::endl;
	};
	std::function<void(Events::HandlersContainerController &)> callback2 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback2" << std::endl;
	};
	std::function<void(Events::HandlersContainerController &)> callback3 =
		[](Events::HandlersContainerController & controller) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback1_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback1" << std::endl;
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback2_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback2" << std::endl;
	};
	Events::TriggerTypes::BeforeMinionSummoned::FunctorType callback3_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};

	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback2_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback1_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback3_2));
	mgr.PushBack(Events::TriggerTypes::BeforeMinionSummoned(callback2_2));

	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(5, Events::TriggerTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(55, Events::TriggerTypes::AfterMinionSummoned(callback2));

	::State::State state;
	CardRef card_ref;
	Cards::RawCard raw_card;
	Cards::Card card(raw_card);
	FlowControl::Context::BeforeMinionSummoned context1(state, card_ref, card);

	Events::StaticEvents<EV1, EV2> static_events;
	std::cout << "=====" << std::endl;
	static_events.Trigger(mgr, context1);
}

int main(void)
{
	test1();
	test2();

	return 0;
}
