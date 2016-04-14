#pragma once

#include "game-ai/board-initializer/board-initializer.h"

namespace JsonBoardFinder
{
	class TreeNode
	{
	public:

	};

	class Tree
	{
	public:
		Tree(BoardInitializer * start_board) : start_board(start_board) {}

	private:
		BoardInitializer * start_board;
	};

} // namespace JsonBoardFinder