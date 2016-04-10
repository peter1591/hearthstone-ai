#pragma once

DEFINE_CARD_CLASS_START(CS2_056)

// Warlock Hero Power: Life Tap

static void HeroPower_Go(Player &player, SlotIndex target)
{
	player.hero.TakeDamage(2, false);
	StageHelper::PlayerDrawCard(player);
}

DEFINE_CARD_CLASS_END()