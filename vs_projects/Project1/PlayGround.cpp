#include <iostream>
#include <vector>
#include <chrono>

#include "Utils/FuncPtrArray.h"


int main(void)
{
	std::cout << "hello" << std::endl;

	typedef void FuncType(int);
	FuncType* foo = [](int a) {
		std::cout << a << std::endl;
	};

	Utils::FuncPtrArray<FuncType*, 3> array1;

	array1(3);

	array1 += foo;
	array1(5);

	array1 += foo;
	array1(10);

	array1 += foo;
	array1(100);

	array1 += foo;
	array1(100);

	return 0;
}