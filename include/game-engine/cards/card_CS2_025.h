#pragma once

DEFINE_CARD_CLASS_START(CS2_025)

// Arcane Explosion

static void Spell_Go(Player &player, SlotIndex)
{
	constexpr bool damage_poisonous = false;
	int damage = 1 + player.GetTotalSpellDamage();

	StageHelper::DealDamage(BoardTargets(player.GetOppositePlayer().minions), damage, damage_poisonous);
}

DEFINE_CARD_CLASS_END()
