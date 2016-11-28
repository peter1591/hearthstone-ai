#pragma once

DEFINE_CARD_CLASS_START(CS2_023)

// Arcane Intellect

static void Spell_Go(Player &player, SlotIndex)
{
	StageHelper::PlayerDrawCard(player);
	StageHelper::PlayerDrawCard(player);
}

DEFINE_CARD_CLASS_END()