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
		// TODO: refine parameter, may be pass board.player?
		SlotIndex opposite_side;
		if (SlotIndexHelper::IsPlayerSide(side)) {
			opposite_side = SlotIndex::SLOT_OPPONENT_SIDE;
		} 
		else {
			opposite_side = SlotIndex::SLOT_PLAYER_SIDE;
		}
		targets = SlotIndexHelper::GetTargets(opposite_side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, board);

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
