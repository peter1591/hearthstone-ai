#pragma once

#include <list>
#include "mcts.h"

class Decider
{
public:
	// only have move constructor, since Merge() will invalidate the mcts state
	Decider(MCTS &&mcts);

	void Merge(MCTS &&mcts);

	void DebugPrint();

private:
	void PrintBestRoute(int levels);

private:
	MCTS &&mcts;
};