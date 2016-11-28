#pragma once

DEFINE_CARD_CLASS_START(CS2_034)

// Mage hero power

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_CHARACTERS);
	meet_requirements = !targets.None();
}

static void HeroPower_Go(Player &player, SlotIndex target)
{
	StageHelper::DealDamage(player.board, target, 1, false);
}

DEFINE_CARD_CLASS_END()