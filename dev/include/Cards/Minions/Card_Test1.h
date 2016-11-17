#pragma once

#include <iostream>

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
			}
		};
	}
}