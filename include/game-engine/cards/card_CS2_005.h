#pragma once

DEFINE_CARD_CLASS_START(CS2_005)

// Claw

static void Spell_Go(Player &player, SlotIndex target)
{
	player.enchantments.Add(std::make_unique<Enchantment_BuffPlayer_C<2, 0, true>>());
	player.hero.GainArmor(2);
}

DEFINE_CARD_CLASS_END()