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

	static void GetRequiredTargetBitmap(GameEngine::Board const& board, TargetorBitmap &targets)
	{
		targets.SetOneTarget(Targetor::GetOpponentHeroIndex());
		for (size_t i = 0; i < board.opponent_minions.GetMinions().size(); ++i)
		{
			targets.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
		}
	}

	static void BattleCry()
	{
		//std::cout << "battle CRY!!!!!" << std::endl;
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
