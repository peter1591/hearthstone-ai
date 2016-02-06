#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/minion.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_002
{
public:
	static constexpr int card_id = CARD_ID_FP1_002;

	// Haunted Creeper

	static void Deathrattle(GameEngine::Board & board, SlotIndex targetor_idx)
	{
		// summon (FP1_002t) * 2 when death
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);
		
		StageHelper::SummonMinion(board, card, targetor_idx);
		StageHelper::SummonMinion(board, card, targetor_idx);
	}

	static void OnSummon(GameEngine::Board const&, SlotIndex, SlotIndex, GameEngine::BoardObjects::Minion & summoning_minion)
	{
		summoning_minion.AddOnDeathTrigger(Deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
