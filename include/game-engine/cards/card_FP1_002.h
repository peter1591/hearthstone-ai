#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

DEFINE_CARD_CLASS_START(FP1_002)
// Haunted Creeper

static void Deathrattle(MinionIterator & triggering_minion)
{
	// summon (FP1_002t) * 2 when death
	Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);
	StageHelper::SummonMinion(card, triggering_minion);
	StageHelper::SummonMinion(card, triggering_minion);
}

DEFINE_CARD_CLASS_END()

#endif