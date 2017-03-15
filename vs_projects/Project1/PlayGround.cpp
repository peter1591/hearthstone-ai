#include <iostream>
#include <utility>

#include "Utils/StaticInvokables.h"

template <int v>
struct StaticInvokable {
	template <typename... Args>
	static void Invoke(Args&&...) {
		std::cout << "invoked: " << v << std::endl;
	}
};

int main(void)
{
	using Utils::StaticInvokableChain;

	typedef StaticInvokable<1> T1;
	typedef StaticInvokable<2> T2;

	void (*func_ptr)(void) = StaticInvokableChain<T1>::CombineType<T2>::Invoke;
	(*func_ptr)();

	return 0;
}