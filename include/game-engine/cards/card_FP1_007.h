#ifndef GAME_ENGINE_CARDS_CARD_FP1_007
#define GAME_ENGINE_CARDS_CARD_FP1_007

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/targetor.h"
#include "game-engine/minion.h"
#include "game-engine/minions.h"
#include "game-engine/stages/helper.h"
#include "game-engine/triggers.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_007
{
public:
	static constexpr int card_id = CARD_ID_FP1_007;

	// Nerubian Egg

	static void Deathrattle(GameEngine::Board & board, Minions & minions, Minions::container_type::iterator it_minion)
	{
		// summon Nerubian (AT_036t) when death at [pos]
	}

	static void OnSummon(GameEngine::Board & board, GameEngine::Minion & summoning_minion)
	{
		// summon Nerubian (AT_036t) when death
		auto deathrattle = new OnDeathTrigger(Deathrattle);
		summoning_minion.AddOnDeathTrigger(deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
