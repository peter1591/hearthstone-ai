#pragma once

#include "judge/IRandomGenerator.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class RandomGenerator : public judge::IRandomGenerator
		{
		public:
			RandomGenerator(SOMCTS & callback) : callback_(callback) {}

			int Get(int exclusive_max) final;

		private:
			SOMCTS & callback_;
		};
	}
}