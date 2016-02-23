#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <stdexcept>
#include "card-base.h"

namespace GameEngine {
namespace Cards {

class Card_CS2_189
{
public:
	static constexpr int card_id = CARD_ID_CS2_189;

	// Elven Archer

	static void GetRequiredTargets(GameEngine::Board const& board, SlotIndex side, SlotIndexBitmap &targets, bool & meet_requirements)
	{
		if (SlotIndexHelper::IsPlayerSide(side)) {
			targets = SlotIndexHelper::GetTargets(SlotIndexHelper::TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, board);
		} 
		else {
			targets = SlotIndexHelper::GetTargets(SlotIndexHelper::TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, board);
		}

		meet_requirements = true; // it's fine even if no target available
	}

	static void BattleCry(GameEngine::Board & board, SlotIndex, GameEngine::Move::PlayMinionData const& play_minion_data)
	{
		constexpr int damage = 1;

#ifdef DEBUG
		if (play_minion_data.target < 0) throw std::runtime_error("logic error");
#endif

		StageHelper::DealDamage(board, play_minion_data.target, damage, false);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
