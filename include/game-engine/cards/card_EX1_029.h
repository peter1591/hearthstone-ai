#pragma once

DEFINE_CARD_CLASS_START(EX1_029)

// Leper Gnome
static void Deathrattle(MinionIterator & triggering_minion)
{
	StageHelper::DealDamage(BoardTargets::EnemyHero(triggering_minion->GetPlayer()), 2, false);
}

static void AfterSummoned(MinionIterator summoned_minion)
{
	summoned_minion->AddOnDeathTrigger(Minion::OnDeathTrigger(Deathrattle));
}

DEFINE_CARD_CLASS_END()
