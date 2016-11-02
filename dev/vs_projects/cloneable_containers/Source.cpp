#include <iostream>
#include <string>
#include <memory>

#include "CloneableContainers/Vector.h"

static void test1()
{
	auto vec1 = std::make_unique<CloneableContainers::Vector<std::string>>();

	auto token1_1 = vec1->Create("item1");
	auto token1_2 = vec1->Create("item2");

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

int main(void)
{
	test1();
	return 0;
}