#pragma once

DEFINE_CARD_CLASS_START(CS2_013)

// Wild Growth

static void Spell_Go(Player &player, SlotIndex target)
{
	player.stat.crystal.GainEmptyCrystals(1);
}

DEFINE_CARD_CLASS_END()