#pragma once

DEFINE_CARD_CLASS_START(EX1_593)

// Nightblade

static void AfterSummoned(MinionIterator summoned_minion)
{
	StageHelper::DealDamage(summoned_minion->GetPlayer().opposite_player.hero, 3, false);
}

DEFINE_CARD_CLASS_END()