#pragma once

#include <map>
#include "game-engine/move.h"

namespace GameEngine {

struct WeightedMove
{
	Move move;
	int accumulated_weight;
};

class WeightedMoves
{
public:
	WeightedMoves() : accumulated_weight(0)
	{

	}

	void Clear()
	{
		this->moves.clear();
		this->accumulated_weight = 0;
	}

	void AddMove(Move const& move, int weight)
	{
		this->accumulated_weight += weight;
		this->moves.insert(std::make_pair(this->accumulated_weight, move));
	}

	bool Choose(unsigned int rand, Move & move)
	{
		unsigned int idx = rand % this->accumulated_weight;
		auto it = this->moves.upper_bound(idx);

		if (it == this->moves.end()) return false;

		move = it->second;
		return true;
	}

private:
	std::map<int, Move> moves;
	int accumulated_weight;
};

} // namespace GameEngine;