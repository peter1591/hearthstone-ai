#ifndef GAME_ENGINE_CARDS_CARD_FP1_007
#define GAME_ENGINE_CARDS_CARD_FP1_007

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"
#include "game-engine/minion.h"
#include "game-engine/minions.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_007
{
public:
	static constexpr int card_id = CARD_ID_FP1_007;

	// Nerubian Egg

	static void Deathrattle(GameEngine::Board & board, int targetor_idx)
	{
		// summon Nerubian (AT_036t) when death at [pos]
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_AT_036t);
		
		GameEngine::Move::PlayMinionData data;
		data.put_location = targetor_idx;
		data.target = -1;

		StageHelper::SummonMinion(board, card, data);
	}

	static void OnSummon(GameEngine::Board const&, int, int, GameEngine::Minion & summoning_minion)
	{
		// summon Nerubian (AT_036t) when death
		summoning_minion.AddOnDeathTrigger(Minion::OnDeathTrigger(Deathrattle));
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
