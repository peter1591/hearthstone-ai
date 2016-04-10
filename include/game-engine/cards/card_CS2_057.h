#pragma once

DEFINE_CARD_CLASS_START(CS2_057)

// Shadow Bolt

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_MINIONS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::DealDamage(player.board, target, 4 + player.GetTotalSpellDamage(), false);
}

DEFINE_CARD_CLASS_END()