#pragma once

DEFINE_CARD_CLASS_START(EX1_246)

// Hex

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_MINIONS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::Transform(player.board, target, CARD_ID_hexfrog);
}

DEFINE_CARD_CLASS_END()