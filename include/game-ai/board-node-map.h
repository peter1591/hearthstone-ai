#pragma once

#include <map>
#include <unordered_set>
#include "game-engine/board.h"
#include "tree.h"

class MCTS;
class StartBoard;

class BoardNodeMap
{
public:
	bool operator==(const BoardNodeMap &rhs) const;
	bool operator!=(const BoardNodeMap &rhs) const;

public:
	void Add(const GameEngine::Board &board, TreeNode *node);
	void Add(std::size_t board_hash, TreeNode *node);
	TreeNode * Find(const GameEngine::Board &board, StartBoard const& start_board) const;

private:
	typedef std::unordered_map<std::size_t, std::unordered_set<TreeNode*> > MapBoardToNodes;

private:
	MapBoardToNodes map; // hash of board --> tree nodes
};