#include <string>
#include <iostream>

#include "StaticDispatcher/Dispatcher.h"

int v;

class Client1
{
public:
	static void BattleCry(const std::string& s)
	{
		v = 3;
		//std::cout << "Client1::BattleCry() called: " << s << std::endl;
	}
};

template <> struct StaticDispatcher::DispatcherMap<0> { typedef Client1 type; };

int main(void)
{
	StaticDispatcher::Dispatcher dispatcher;

	v = 0;

	dispatcher.BattleCry(3, "asd");
	std::cout << v << std::endl;

	dispatcher.BattleCry(0, "asdasd");
	std::cout << v << std::endl;

	return 0;
}
