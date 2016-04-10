#pragma once

DEFINE_CARD_CLASS_START(CS2_062)

// Hellfire

static void Spell_Go(Player &player, SlotIndex target)
{
	StageHelper::DealDamage(BoardTargets::AllCharacters(player.board), 3 + player.GetTotalSpellDamage(), false);
}

DEFINE_CARD_CLASS_END()