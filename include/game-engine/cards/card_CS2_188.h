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
		Targetor::TargetType target_type;

		if (board.GetStageType() == GameEngine::STAGE_TYPE_PLAYER)
			target_type = Targetor::TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL;
		else
			target_type = Targetor::TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL;

		targets = Targetor::GetTargets(target_type, board);
		meet_requirements = true; // it's fine even if no target available
	}

	static void BattleCry(GameEngine::Board & board)
	{
		constexpr int attack_boost = 2;

		int target_idx = board.data.player_play_minion_data.required_target;
		if (target_idx < 0) {
			// no target to buff
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
