#include <iostream>
#include <string>
#include <memory>

#include "CloneableContainers/Vector.h"

int main(void)
{
	auto vec1 = std::make_unique<CloneableContainers::Vector<std::string>>();

	auto token1_1 = vec1->Create("item1");
	auto token1_2 = vec1->Create("item2");

	std::cout << "==" << std::endl;
	std::cout << "token1_1 = " << vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << vec1->Get(token1_2) << std::endl;

	std::cout << "==" << std::endl;
	auto vec2 = *vec1;
	auto token2_1 = token1_1;
	auto token2_2 = token1_2;
	std::cout << "token1_1 = " << vec1->Get(token1_1) << std::endl;
	std::cout << "token1_2 = " << vec1->Get(token1_2) << std::endl;
	std::cout << "token2_1 = " << vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << vec2.Get(token2_2) << std::endl;

	std::cout << "==" << std::endl;
	vec1.reset();
	std::cout << "token2_1 = " << vec2.Get(token2_1) << std::endl;
	std::cout << "token2_2 = " << vec2.Get(token2_2) << std::endl;

	return 0;
}