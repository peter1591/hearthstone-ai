#include <string>
#include <iostream>

#include "Cards/Minions/Dispatcher.h"

int main(void)
{
	Cards::Minions::Dispatcher::BattleCry(2, "jjj");
	Cards::Minions::Dispatcher::BattleCry(7, "jssjj");

	return 0;
}
