#ifndef GAME_ENGINE_CARDS_CARD_FP1_007
#define GAME_ENGINE_CARDS_CARD_FP1_007

DEFINE_CARD_CLASS_START(FP1_007)

// Nerubian Egg

static void Deathrattle(MinionIterator & triggering_minion)
{
	// summon Nerubian (AT_036t) on death
	Card card = CardDatabase::GetInstance().GetCard(CARD_ID_AT_036t);
	StageHelper::SummonMinion(card, triggering_minion);
}

static void AfterSummoned(MinionIterator summoned_minion)
{
	summoned_minion->AddOnDeathTrigger(Minion::OnDeathTrigger(Deathrattle));
}

DEFINE_CARD_CLASS_END()

#endif
