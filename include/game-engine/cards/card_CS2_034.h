#pragma once

DEFINE_CARD_CLASS_START(CS2_034)

// Mage hero power

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, player.board);
	meet_requirements = !targets.None();
}

static void HeroPower_Go(Player &player, SlotIndex target)
{
	StageHelper::DealDamage(player.board, target, 1, false);
}

DEFINE_CARD_CLASS_END()