#pragma once

#include "state/Types.h"
#include "state/board/Player.h"

namespace state
{
	namespace board
	{
		class Board
		{
		public:
			Board() : minion_cost_extra_(0) {}

			Player & Get(PlayerIdentifier identifier)
			{
				if (identifier.IsFirst()) return first_;
				else return second_;
			}

			const Player & Get(PlayerIdentifier identifier) const
			{
				if (identifier.IsFirst()) return first_;
				else return second_;
			}

			Player & GetFirst() { return first_; }
			Player const& GetFirst() const { return first_; }

			Player & GetSecond() { return second_; }
			Player const& GetSecond() const { return second_; }

			int minion_cost_extra_;

		private:
			Player first_;
			Player second_;
		};
	}
}