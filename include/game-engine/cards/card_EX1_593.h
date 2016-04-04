#pragma once

DEFINE_CARD_CLASS_START(EX1_593)

// Nightblade

static void BattleCry(Player & player, Move::PlayMinionData const&)
{
	StageHelper::DealDamage(player.opposite_player.hero, 3, false);
}

DEFINE_CARD_CLASS_END()