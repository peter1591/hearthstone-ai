#include <string>
#include <iostream>
#include <assert.h>

#include "StaticDispatcher/MinionDispatcher.h"

int test;

namespace Cards
{
	namespace Minions
	{
		class Card_Test1
		{
		public:
			static constexpr int id = 7;

			static void BattleCry()
			{
				std::cout << "Card_Test1::BattleCry() called" << std::endl;
				test += 1;
			}
		};
		class Card_Test2
		{
		public:
			static constexpr int id = 55;

			static void BattleCry()
			{
				test += 2;
			}
		};
		class Card_Test3
		{
		public:
			static constexpr int id = 58;
		};
	}
}

REGISTER_MINION_CARD_CLASS(Cards::Minions::Card_Test1)
REGISTER_MINION_CARD_CLASS(Cards::Minions::Card_Test2)
REGISTER_MINION_CARD_CLASS(Cards::Minions::Card_Test3)

int main(void)
{
	test = 0;

	Cards::Minions::Dispatcher::BattleCry(2);
	assert(test == 0);

	Cards::Minions::Dispatcher::BattleCry(7);
	assert(test == 1);

	Cards::Minions::Dispatcher::BattleCry(7);
	assert(test == 2);

	Cards::Minions::Dispatcher::BattleCry(4);
	assert(test == 2);

	Cards::Minions::Dispatcher::BattleCry(55);
	assert(test == 4);

	Cards::Minions::Dispatcher::BattleCry(58);
	assert(test == 4);

	return 0;
}
