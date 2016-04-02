#ifndef GAME_ENGINE_CARDS_CARD_CS2_188
#define GAME_ENGINE_CARDS_CARD_CS2_188

#include "card-base.h"

namespace GameEngine {
namespace Cards {

class Card_CS2_188
{
public:
	static constexpr int card_id = CARD_ID_CS2_188;

	// Abusive Sergant

	static void GetRequiredTargets(GameEngine::Board const& board, SlotIndex side, SlotIndexBitmap &targets, bool & meet_requirements)
	{
		targets = SlotIndexHelper::GetTargets(side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL, board);
		meet_requirements = true; // it's fine even if no target available
	}

	static void BattleCry(GameEngine::Board & board, SlotIndex, GameEngine::Move::PlayMinionData const& play_minion_data)
	{
		constexpr int attack_boost = 2;

		if (play_minion_data.target < 0) {
			// no target to buff
			return;
		}

		try {
			auto & buff_target = board.object_manager.GetMinion(play_minion_data.target);
			auto enchant = std::make_unique<BoardObjects::Enchantment_BuffMinion_C<attack_boost, 0, 0, true>>();
			buff_target.AddEnchantment(std::move(enchant), nullptr);
		}
		catch (std::out_of_range ex) {
#ifdef DEBUG
			std::cerr << "WARNING: buff target vanished." << std::endl;
#endif
			return;
		}
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
