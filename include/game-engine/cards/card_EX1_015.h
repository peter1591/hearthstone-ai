#pragma once

DEFINE_CARD_CLASS_START(EX1_015)

// Novice Enginner

static void BattleCry(Player & player, Move::PlayMinionData const&)
{
	StageHelper::PlayerDrawCard(player);
}

DEFINE_CARD_CLASS_END()