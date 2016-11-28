#include <string>
#include <iostream>
#include <assert.h>

#include "FlowControl/Dispatchers/Minions.h"

int test;

namespace Cards
{
	namespace Minions
	{
		class Card_Test1
		{
		public:
			static void BattleCry()
			{
				std::cout << "Card_Test1::BattleCry() called" << std::endl;
				test += 1;
			}
		};
		class Card_Test2
		{
		public:
			static void BattleCry()
			{
				test += 2;
			}
		};
		class Card_Test3
		{
		public:
			static constexpr int id = 57;
		};
	}
}

REGISTER_MINION_CARD_CLASS(7, Cards::Minions::Card_Test1)
REGISTER_MINION_CARD_CLASS(55,Cards::Minions::Card_Test2)
REGISTER_MINION_CARD_CLASS(57, Cards::Minions::Card_Test3)

int main(void)
{
	test = 0;

	FlowControl::Dispatchers::Minions::BattleCry(2);
	assert(test == 0);

	FlowControl::Dispatchers::Minions::BattleCry(7);
	assert(test == 1);

	FlowControl::Dispatchers::Minions::BattleCry(7);
	assert(test == 2);

	FlowControl::Dispatchers::Minions::BattleCry(4);
	assert(test == 2);

	FlowControl::Dispatchers::Minions::BattleCry(55);
	assert(test == 4);

	FlowControl::Dispatchers::Minions::BattleCry(58);
	assert(test == 4);

	return 0;
}
