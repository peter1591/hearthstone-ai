#include <iostream>
#include <vector>
#include <chrono>

#include "state/State.h"

#include "Utils/UnorderedInvokables.h"

class TestEvent_Ptr
{
public:
	struct Context
	{
		int& counter;
	};

public:
	typedef void(*FunctorType)(state::Events::HandlersContainerController &, state::CardRef, Context &&);
	typedef std::tuple<state::CardRef, Context &&> ArgsTuple;

	template <typename T,
		typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
		explicit TestEvent_Ptr(T&& functor) : functor_(functor) {}

	void Handle(state::Events::HandlersContainerController &controller, state::CardRef card_ref, Context && context)
	{
		functor_(controller, card_ref, std::move(context));
	}

private:
	FunctorType functor_;
};
class TestEvent_Functor
{
public:
	struct Context
	{
		int& counter;
	};

public:
	typedef std::function<void(state::Events::HandlersContainerController &, Context &&)> FunctorType;
	typedef std::tuple<Context &&> ArgsTuple;

	template <typename T,
		typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
		explicit TestEvent_Functor(T&& functor) : functor_(functor) {}

	void Handle(state::Events::HandlersContainerController &controller, Context && context)
	{
		functor_(controller, std::move(context));
	}

private:
	FunctorType functor_;
};

int main(void)
{
	std::cout << "hello" << std::endl;

	int counter1 = 0;
	int counter2 = 0;
	std::vector<state::CardRef> card_refs;

	for (int i = 0; i < 10; ++i) {
		state::CardRef card_ref;
		//card_ref.id.SetDebug(i*1);
		card_refs.push_back(card_ref);
	}

	state::Events::impl::HandlersContainer<TestEvent_Functor> container1;
	state::Events::impl::CategorizedHandlersContainer<TestEvent_Ptr> container2;

	for (auto card_ref : card_refs) {
		container1.PushBack([card_ref](auto& controller, auto&& context) {
			context.counter += 10;
		});

		container2.PushBack(card_ref, [](auto& controller, state::CardRef ref, auto&& context) {
			context.counter += 10;
		});
	}

	constexpr int times = 10000000;

	while (true) {
		counter1 = 0;
		counter2 = 0;

		auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < times; ++i) {
			container1.TriggerAll(TestEvent_Functor::Context{ counter1 });
		}
		double ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		std::cout << "[functor] elapsed: " << ms << " ms" << std::endl;
		std::cout << "counter1 = " << counter1 << std::endl;

		start = std::chrono::steady_clock::now();
		state::CardRef card_ref;
		for (int i = 0; i < times; ++i) {
			container2.TriggerAllCategories(card_ref, TestEvent_Ptr::Context{ counter2 });
		}
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		std::cout << "[ptr] elapsed: " << ms << " ms" << std::endl;
		std::cout << "counter2 = " << counter2 << std::endl;
	}

	return 0;
}