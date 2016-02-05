#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"
#include "game-engine/minion.h"
#include "game-engine/minions.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_002
{
public:
	static constexpr int card_id = CARD_ID_FP1_002;

	// Haunted Creeper

	static void Deathrattle(GameEngine::Board & board, int targetor_idx)
	{
		// summon (FP1_002t) * 2 when death
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);
		
		GameEngine::Move::PlayMinionData data;
		data.put_location = targetor_idx;
		data.target = -1;

		StageHelper::SummonMinion(board, card, data);
		StageHelper::SummonMinion(board, card, data);
	}

	static void OnSummon(GameEngine::Board const&, int, int, GameEngine::Minion & summoning_minion)
	{
		summoning_minion.AddOnDeathTrigger(Deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
