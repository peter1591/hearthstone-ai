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

	static void GetRequiredTargets(GameEngine::Board const& board, int playing_hero, TargetorBitmap &targets, bool & meet_requirements)
	{
		Targetor::TargetType target_type;

		if (playing_hero == Targetor::GetPlayerHeroIndex()) {
			target_type = Targetor::TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL;
		}
		else {
			target_type = Targetor::TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL;
		}

		targets = Targetor::GetTargets(target_type, board);
		meet_requirements = true; // it's fine even if no target available
	}

	static void BattleCry(GameEngine::Board & board, GameEngine::Move::PlayMinionData const& play_minion_data)
	{
		constexpr int attack_boost = 2;

		if (play_minion_data.target < 0) {
			// no target to buff
			return;
		}

		Minions *minions;
		auto minion_it = GameEngine::StageHelper::GetMinionIterator(board, play_minion_data.target, minions);
		minion_it->AddAttackThisTurn(attack_boost);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
