#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

#include "card-base.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_002
{
public:
	static constexpr int card_id = CARD_ID_FP1_002;

	// Haunted Creeper

	static void Deathrattle(GameEngine::BoardObjects::MinionIterator & triggering_minion)
	{
		// summon (FP1_002t) * 2 when death
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);
		StageHelper::SummonMinion(card, triggering_minion);
		StageHelper::SummonMinion(card, triggering_minion);
	}

	static void AfterSummoned(GameEngine::BoardObjects::MinionIterator summoned_minion)
	{
		summoned_minion->AddOnDeathTrigger(GameEngine::BoardObjects::Minion::OnDeathTrigger(Deathrattle));
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
