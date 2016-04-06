#pragma once

DEFINE_CARD_CLASS_START(CS2_022)

// Polymorph

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_ENEMY_SPELL, player.board);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::Transform(player.board, target, CARD_ID_CS2_tk1);
}

DEFINE_CARD_CLASS_END()