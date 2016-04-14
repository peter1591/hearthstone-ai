#pragma once

#include "json/value.h"
#include "game-engine/board.h"

namespace JsonBoardFinder
{
	class BoardComparator
	{
	public:
		static bool IsEqual(GameEngine::Board const& board, Json::Value const& json_board)
		{
			// TODO
			return false;
		}
	};
} // namespace JsonBoardFinder