#pragma once

DEFINE_CARD_CLASS_START(EX1_169)

// Innervate

static void Spell_Go(Player &player, SlotIndex target)
{
	player.stat.crystal.IncreaseCrystalThisTurn(2);
}

DEFINE_CARD_CLASS_END()