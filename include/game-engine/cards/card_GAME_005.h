#pragma once

DEFINE_CARD_CLASS_START(GAME_005)

// The Coin

static void Spell_Go(Player &player, SlotIndex)
{
	player.stat.crystal.IncreaseCrystalThisTurn(1);
}

DEFINE_CARD_CLASS_END()