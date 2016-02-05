#pragma once

#include "game-engine/board.h"
#include "game-engine/minions.h"

namespace GameEngine {

	class OnDeathTrigger
	{
	public:
		typedef void Func(Board & board, Minions & minions, Minions::container_type::iterator it_minion);

	public:
		OnDeathTrigger(Func* func) : func(func) {}

		void operator()(Board & board, Minions & minions, Minions::container_type::iterator it_minion) {
			return this->func(board, minions, it_minion);
		}

	private:
		Func * func;
	};

}