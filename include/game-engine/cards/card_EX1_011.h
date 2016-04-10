#pragma once

DEFINE_CARD_CLASS_START(EX1_011)

// Voodoo Doctor

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_CHARACTERS);
	meet_requirements = true; // it's fine even if no target available
}

static void BattleCry(Player & player, Move::PlayMinionData const& play_minion_data)
{
	StageHelper::Heal(player.board, play_minion_data.target, 2);
}

DEFINE_CARD_CLASS_END()