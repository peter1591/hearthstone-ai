#ifndef GAME_ENGINE_CARDS_CARD_CS2_188
#define GAME_ENGINE_CARDS_CARD_CS2_188

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_CS2_188
{
public:
	static constexpr int card_id = CARD_ID_CS2_188;

	// Abusive Sergant

	static void GetRequiredTargets(GameEngine::Board const& board, TargetorBitmap &targets, bool & meet_requirements)
	{
		for (int i = 0; i < (int)board.player_minions.GetMinions().size(); ++i)
		{
			targets.SetOneTarget(Targetor::GetPlayerMinionIndex(i));
		}
		meet_requirements = true; // playable even if no target is available
	}

	static void BattleCry(GameEngine::Board & board)
	{
		constexpr int attack_boost = -2;

		int target_idx = board.data.player_put_minion_data.required_target;
		if (target_idx < 0) {
			// no target
			return;
		}

		Minions *minions;
		auto minion_it = StageHelper::GetMinionIterator(board, target_idx, minions);
		minion_it->AddAttackThisTurn(attack_boost);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
