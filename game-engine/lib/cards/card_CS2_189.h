#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <iostream>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"

namespace GameEngine {
namespace Cards {

class Card_CS2_189
{
public:
	static constexpr int card_id = CARD_ID_CS2_189;

	// Elven Archer

	static void GetRequiredTargets(GameEngine::Board const& board, TargetorBitmap &targets, bool & meet_requirements)
	{
		targets.SetOneTarget(Targetor::GetOpponentHeroIndex());
		for (size_t i = 0; i < board.opponent_minions.GetMinions().size(); ++i)
		{
			targets.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
		}
		meet_requirements = true;
	}

	static void BattleCry(GameEngine::Board & board)
	{

	}
};

} // namespace Cards
} // namespace GameEngine

#endif
