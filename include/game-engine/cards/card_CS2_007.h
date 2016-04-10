#pragma once

DEFINE_CARD_CLASS_START(CS2_007)

// Healing Touch

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_CHARACTERS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::Heal(player.board, target, 8);
}

DEFINE_CARD_CLASS_END()