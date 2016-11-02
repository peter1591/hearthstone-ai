#include <iostream>
#include <string>
#include <memory>

#include "CloneableContainers/Vector.h"
#include "CloneableContainers/RemovableVector.h"
#include "CloneableContainers/CloneByCopyWrapper.h"

static void test1()
{
	auto vec1 = std::make_unique<CloneableContainers::Vector<std::string>>();

	auto token1_1 = vec1->PushBack("item1");
	auto token1_2 = vec1->PushBack("item2");

	std::cout << "==" << std::endl;
	std::cout << "token1_1 = " << vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << vec1->Get(token1_2) << std::endl;

	for (auto it = vec1->GetFirst(); !vec1->ReachedEnd(it); vec1->StepNext(it)) {
		std::cout << "Iterated vec1: " << vec1->Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	auto vec2 = *vec1;
	auto token2_1 = token1_1;
	auto token2_2 = token1_2;
	std::cout << "token1_1 = " << vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << vec1->Get(token1_2) << std::endl;
	std::cout << "token2_1 = " << vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << vec2.Get(token2_2) << std::endl;
	for (auto it = vec1->GetFirst(); !vec1->ReachedEnd(it); vec1->StepNext(it)) {
		std::cout << "Iterated vec1: " << vec1->Get(it) << std::endl;
	}
	for (auto it = vec2.GetFirst(); !vec2.ReachedEnd(it); vec2.StepNext(it)) {
		std::cout << "Iterated vec2: " << vec2.Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	vec1.reset();
	std::cout << "token2_1 = " << vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << vec2.Get(token2_2) << std::endl;
	for (auto it = vec2.GetFirst(); !vec2.ReachedEnd(it); vec2.StepNext(it)) {
		std::cout << "Iterated vec2: " << vec2.Get(it) << std::endl;
	}

}

static void test2()
{
	auto vec1 = std::make_unique<CloneableContainers::RemovableVector<std::string>>();

	auto token1_1 = vec1->PushBack("item1");
	auto token1_2 = vec1->PushBack("item2");

	std::cout << "==" << std::endl;
	std::cout << "token1_1 = " << *vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << *vec1->Get(token1_2) << std::endl;
	for (auto it = vec1->GetFirst(); !vec1->ReachedEnd(it); vec1->StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1->Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	auto vec2 = *vec1;
	auto token2_1 = token1_1;
	auto token2_2 = token1_2;
	std::cout << "token1_1 = " << *vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << *vec1->Get(token1_2) << std::endl;
	std::cout << "token2_1 = " << *vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << *vec2.Get(token2_2) << std::endl;
	for (auto it = vec1->GetFirst(); !vec1->ReachedEnd(it); vec1->StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1->Get(it) << std::endl;
	}
	for (auto it = vec2.GetFirst(); !vec2.ReachedEnd(it); vec2.StepNext(it)) {
		std::cout << "Iterated vec2: " << *vec2.Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	vec1.reset();
	std::cout << "token2_1 = " << *vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << *vec2.Get(token2_2) << std::endl;
	for (auto it = vec2.GetFirst(); !vec2.ReachedEnd(it); vec2.StepNext(it)) {
		std::cout << "Iterated vec2: " << *vec2.Get(it) << std::endl;
	}
}

static void test3()
{
	CloneableContainers::RemovableVector<std::string> vec1;

	auto token1 = vec1.PushBack("first");
	auto token2 = vec1.PushBack("second");

	for (auto it = vec1.GetFirst(); !vec1.ReachedEnd(it); vec1.StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1.Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	auto token3 = vec1.PushBack("third");
	for (auto it = vec1.GetFirst(); !vec1.ReachedEnd(it); vec1.StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1.Get(it) << std::endl;
	}

	auto vec2 = vec1;

	std::cout << "==" << std::endl;
	vec1.Remove(token2);
	std::cout << "get a removed token: " << vec1.Get(token2) << std::endl;
	std::cout << "get the token from copied container: " << *vec2.Get(token2) << std::endl;
	for (auto it = vec1.GetFirst(); !vec1.ReachedEnd(it); vec1.StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1.Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	vec1.PushBack("forth");
	for (auto it = vec1.GetFirst(); !vec1.ReachedEnd(it); vec1.StepNext(it)) {
		std::cout << "Iterated vec1: " << *vec1.Get(it) << std::endl;
	}

	std::cout << "==" << std::endl;
	for (auto it = vec2.GetFirst(); !vec2.ReachedEnd(it); vec2.StepNext(it)) {
		std::cout << "Iterated vec2: " << *vec2.Get(it) << std::endl;
	}
}

class Wrap1
{
public:
	explicit Wrap1(std::unique_ptr<int> v) : v_(std::move(v)) {}

	friend std::ostream& operator<<(std::ostream& os, const Wrap1 & v)
	{
		os << "[@" << v.v_.get() << "]: " << *v.v_;
		return os;
	}

	Wrap1 Clone() const
	{
		return Wrap1(std::make_unique<int>(*this->v_));
	}

private:
	std::unique_ptr<int> v_;
};

static void test4()
{
	Wrap1 rawitem1(std::make_unique<int>(2002));
	CloneByCopyWrapper<Wrap1> item1(std::move(rawitem1));

	CloneableContainers::RemovableVector<CloneByCopyWrapper<Wrap1>> vec1;
	auto token1 = vec1.PushBack(std::move(item1));
	std::cout << "==" << std::endl;
	std::cout << "value: " << vec1.Get(token1)->Get() << std::endl;

	auto vec2 = vec1;
	std::cout << "==" << std::endl;
	std::cout << "value: " << vec1.Get(token1)->Get() << std::endl;
	std::cout << "value: " << vec2.Get(token1)->Get() << std::endl;

	vec1.Remove(token1);
	std::cout << "==" << std::endl;
	std::cout << "value 1: @" << vec1.Get(token1) << std::endl;
	std::cout << "value 2: @" << vec2.Get(token1) << std::endl;
	std::cout << "value 2: " << vec2.Get(token1)->Get() << std::endl;
}

int main(void)
{
	//test1();
	//test2();
	//test3();
	test4();
	return 0;
}