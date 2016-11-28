#pragma once

DEFINE_CARD_CLASS_START(CS2_037)

// Frost shock

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ENEMY_CHARACTERS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::DealDamage(player.board, target, 1 + player.GetTotalSpellDamage(), false);
	StageHelper::Freeze(player.board, target);
}

DEFINE_CARD_CLASS_END()