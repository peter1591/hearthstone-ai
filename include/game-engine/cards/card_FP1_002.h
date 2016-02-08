#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_002
{
public:
	static constexpr int card_id = CARD_ID_FP1_002;

	// Haunted Creeper

	static void Deathrattle(GameEngine::Board & board, GameEngine::BoardObjects::MinionsIteratorWithIndex triggering_minion)
	{
		// summon (FP1_002t) * 2 when death
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);
		
		StageHelper::SummonMinion(board, card, triggering_minion);
		StageHelper::SummonMinion(board, card, triggering_minion);
	}

	static void AfterSummoned(GameEngine::Board &, GameEngine::BoardObjects::Minion & summoned_minion)
	{
		summoned_minion.AddOnDeathTrigger(Deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
