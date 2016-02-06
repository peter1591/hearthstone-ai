#ifndef GAME_ENGINE_CARDS_CARD_FP1_007
#define GAME_ENGINE_CARDS_CARD_FP1_007

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/minion.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_007
{
public:
	static constexpr int card_id = CARD_ID_FP1_007;

	// Nerubian Egg

	static void Deathrattle(GameEngine::Board & board, SlotIndex targetor_idx)
	{
		// summon Nerubian (AT_036t) when death at [pos]
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_AT_036t);
		
		StageHelper::SummonMinion(board, card, targetor_idx);
	}

	static void OnSummon(GameEngine::Board const&, SlotIndex, SlotIndex, GameEngine::BoardObjects::Minion & summoning_minion)
	{
		// summon Nerubian (AT_036t) when death
		summoning_minion.AddOnDeathTrigger(Deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
