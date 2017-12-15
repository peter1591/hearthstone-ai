#pragma once

#include "state/IRandomGenerator.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class RandomGenerator : public state::IRandomGenerator
		{
		public:
			RandomGenerator(SOMCTS & callback) : callback_(callback) {}

			int Get(int exclusive_max) final;

		private:
			SOMCTS & callback_;
		};
	}
}