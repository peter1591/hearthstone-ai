#pragma once

DEFINE_CARD_CLASS_START(CS2_017)

// Druid hero power

static void HeroPower_Go(Player &player, SlotIndex target)
{
	player.enchantments.Add(std::make_unique<Enchantment_BuffPlayer_C<1, 0, true>>());
	player.hero.GainArmor(1);
}

DEFINE_CARD_CLASS_END()