#pragma once

DEFINE_CARD_CLASS_START(EX1_029)

// Leper Gnome
static void Deathrattle(MinionIterator & triggering_minion)
{
	// deal 2 damage to opponent hero
	constexpr int damage = 2;

	auto & board = triggering_minion.GetBoard();

	if (triggering_minion.IsPlayerSide()) {
		StageHelper::DealDamage(board.opponent.hero, damage, false);
	}
	else {
		StageHelper::DealDamage(board.player.hero, damage, false);
	}
}

static void AfterSummoned(MinionIterator summoned_minion)
{
	summoned_minion->AddOnDeathTrigger(Minion::OnDeathTrigger(Deathrattle));
}

DEFINE_CARD_CLASS_END()
