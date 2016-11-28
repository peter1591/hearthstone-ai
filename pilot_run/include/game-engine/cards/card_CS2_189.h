#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <stdexcept>

DEFINE_CARD_CLASS_START(CS2_189)

// Elven Archer

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_CHARACTERS);
	meet_requirements = true; // it's fine even if no target available
}

static void BattleCry(Player & player, Move::PlayMinionData const& play_minion_data)
{
	StageHelper::DealDamage(player.board, play_minion_data.target, 1, false);
}

DEFINE_CARD_CLASS_END()

#endif
