#pragma once

#include "state/board/Board.h"
#include "state/Cards/Manager.h"

namespace state
{
	namespace detail
	{
		class ReplaceHelper
		{
		public:
			ReplaceHelper(board::Board & board, Cards::Manager & cards) : board_(board), cards_(cards) {}

			void Replace(CardRef source, CardRef destination)

		private:
			board::Board & board_;
			Cards::Manager & cards_;
		};
	}
}