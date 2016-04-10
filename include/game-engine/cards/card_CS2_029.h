#pragma once

DEFINE_CARD_CLASS_START(CS2_029)

// Fireball

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_CHARACTERS);
	meet_requirements = !targets.None();
}

static void Spell_Go(Player &player, SlotIndex target)
{
#ifdef DEBUG
	if (target < 0) throw std::runtime_error("logic error");
#endif

	int damage = 6 + player.GetTotalSpellDamage();
	StageHelper::DealDamage(player.board, target, damage, false);
}

DEFINE_CARD_CLASS_END()