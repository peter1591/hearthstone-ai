#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <list>

#include "Utils/InvokableWrapper.h"
#include "Utils/Invokables.h"
#include "Utils/UnorderedInvokables.h"

using Utils::Invokables;
using Utils::UnorderedInvokables;

void func1(int* counter)
{
	++(*counter);
}

class AddTest
{
public:
	AddTest(Invokables<void, int*>& list1, UnorderedInvokables<void, int*>& list2, std::vector<std::function<void(int*)>>& list3)
		: list1_(list1), list2_(list2), list3_(list3)
	{}

	template <typename T>
	void Add(T&& item) {
		list1_.PushBack(std::forward<T>(item));
		list2_.PushBack(std::forward<T>(item));
		list3_.push_back(std::forward<T>(item));
	}

private:
	Invokables<void, int*> & list1_;
	UnorderedInvokables<void, int*> & list2_;
	std::vector<std::function<void(int*)>> & list3_;
};

void test()
{
	int counter1 = 0;

	volatile int times = 10000000;

	//std::vector<Invokable<void, int*>> functor_list1;
	//std::vector<void(*)(int*)> functor_list1;
	Invokables<void, int*> functor_list1;
	UnorderedInvokables<void, int*> functor_list2;
	std::vector<std::function<void(int*)>> functor_list3;

	AddTest test_adder(functor_list1, functor_list2, functor_list3);
	//for (int i=0; i<10; ++i) test_adder.Add(func1);
	for (int i = 0; i<10; ++i) test_adder.Add([](int* counter1) { ++(*counter1); });
	for (int i = 0; i<10; ++i) test_adder.Add([&counter1](int* counter1_) { ++counter1; });

	auto start = std::chrono::steady_clock::now();
	counter1 = 0;
	for (int i = 0; i < times; ++i) {
		functor_list1.InvokeAll(&counter1);
	}
	std::cout << "invokables costs: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
		<< " ms" << std::endl;
	std::cout << counter1 << std::endl;

	start = std::chrono::steady_clock::now();
	counter1 = 0;
	for (int i = 0; i < times; ++i) {
		functor_list2.InvokeAll(&counter1);
	}
	std::cout << "unordered invokables costs: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
		<< " ms" << std::endl;
	std::cout << counter1 << std::endl;

	start = std::chrono::steady_clock::now();
	counter1 = 0;
	for (int i = 0; i < times; ++i) {
		for (auto const& item : functor_list3) item(&counter1);
	}
	std::cout << "std::function costs: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
		<< " ms" << std::endl;

	std::cout << counter1 << std::endl;
}

int main(void)
{
	std::cout << "hello" << std::endl;

	while (true) {
		test();
	}
}