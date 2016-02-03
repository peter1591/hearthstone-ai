#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <iostream>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"
#include "game-engine/stages/helper.h"

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
		for (int i = 0; i < (int)board.opponent_minions.GetMinions().size(); ++i)
		{
			targets.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
		}
		meet_requirements = true;
	}

	static void BattleCry(GameEngine::Board & board)
	{
		constexpr int damage = 1;
		int damage_taker_idx = board.data.player_put_minion_data.required_target;

		StageHelper::TakeDamage(board, damage_taker_idx, damage);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
