#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <stdexcept>
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
		if (board.GetStageType() == GameEngine::STAGE_TYPE_PLAYER) {
			targets = Targetor::GetTargets(Targetor::TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, board);
		} 
		else {
			targets = Targetor::GetTargets(Targetor::TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, board);
		}

		meet_requirements = true; // it's fine even if no target available
	}

	static void BattleCry(GameEngine::Board & board)
	{
		constexpr int damage = 1;
		int damage_taker_idx = board.data.player_put_minion_data.required_target;

#ifdef DEBUG
		if (damage_taker_idx < 0) throw std::runtime_error("logic error");
#endif

		StageHelper::TakeDamage(board, damage_taker_idx, damage);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
