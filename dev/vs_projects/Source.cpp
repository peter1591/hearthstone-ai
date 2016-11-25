#include <iostream>
#include <functional>

#include "State/Events/Manager.h"
#include "State/Events/Event.h"
#include "FlowControl/Context/BeforeMinionSummoned.h"
#include "State/State.h"
#include "State/Cards/Card.h"

using namespace state;

static void test1()
{
	Events::Manager mgr;

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
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback1_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback1" << std::endl;
	};
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback2_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback2" << std::endl;
	};
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback3_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};

	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback2_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback1_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback3_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback2_2));

	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback2));

	auto mgr2 = mgr;

	state::State state;
	CardRef card_ref;
	Cards::RawCard raw_card;
	Cards::Card card(raw_card);
	FlowControl::Context::BeforeMinionSummoned context1(state, card_ref, card);

	Events::Event<Events::EventTypes::AfterMinionSummoned> ev1;
	Events::Event<Events::EventTypes::BeforeMinionSummoned> ev2(context1);

	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	mgr.TriggerEvent<Events::EventTypes::AfterMinionSummoned>();
	std::cout << "--" << std::endl;
	mgr.TriggerEvent<Events::EventTypes::BeforeMinionSummoned>(context1);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr);

	std::cout << "--" << std::endl;
	mgr2.TriggerEvent<Events::EventTypes::AfterMinionSummoned>();
	std::cout << "--" << std::endl;
	ev1.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	ev2.TriggerEvent(mgr2);
	std::cout << "--" << std::endl;
	mgr2.TriggerEvent<Events::EventTypes::BeforeMinionSummoned>(context1);

	Events::CategorizedEvent<Events::EventTypes::AfterMinionSummoned> ev3(4);
	Events::CategorizedEvent<Events::EventTypes::AfterMinionSummoned> ev4(55);

	std::cout << "--" << std::endl;
	mgr.TriggerCategorizedEvent<Events::EventTypes::AfterMinionSummoned>(4);
	std::cout << "--" << std::endl;
	ev3.TriggerEvent(mgr);
	std::cout << "--" << std::endl;
	mgr.TriggerCategorizedEvent<Events::EventTypes::AfterMinionSummoned>(55);
	std::cout << "--" << std::endl;
	ev4.TriggerEvent(mgr);

	std::cout << "---" << std::endl;
	ev3.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	mgr2.TriggerCategorizedEvent<Events::EventTypes::AfterMinionSummoned>(4);
	std::cout << "---" << std::endl;
	ev4.TriggerEvent(mgr2);
	std::cout << "---" << std::endl;
	mgr2.TriggerCategorizedEvent<Events::EventTypes::AfterMinionSummoned>(55);
}

static void test2()
{
	Events::Manager mgr;

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
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback1_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback1" << std::endl;
	};
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback2_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback2" << std::endl;
	};
	Events::EventTypes::BeforeMinionSummoned::FunctorType callback3_2 =
		[](Events::HandlersContainerController & controller, FlowControl::Context::BeforeMinionSummoned & context) {
		std::cout << "callback3" << std::endl;
		controller.Remove();
	};

	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(Events::EventTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback2_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback1_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback3_2));
	mgr.PushBack(Events::EventTypes::BeforeMinionSummoned(callback2_2));

	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(5, Events::EventTypes::AfterMinionSummoned(callback2));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback1));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback3));
	mgr.PushBack(55, Events::EventTypes::AfterMinionSummoned(callback2));

	state::State state;
	CardRef card_ref;
	Cards::RawCard raw_card;
	Cards::Card card(raw_card);
	FlowControl::Context::BeforeMinionSummoned context1(state, card_ref, card);
}

int main(void)
{
	test1();
	test2();

	return 0;
}
