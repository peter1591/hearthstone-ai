#include <iostream>

template <typename T>
void foo(T&& t)
{
}

int main(void)
{
	foo([]() {
		std::cout << "hello " << std::endl;
	});
	return 0;
}