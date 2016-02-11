#ifndef GAME_ENGINE_CARDS_CARD_FP1_002
#define GAME_ENGINE_CARDS_CARD_FP1_002

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion-manipulator.h"

namespace GameEngine {
namespace Cards {

class Card_FP1_002
{
public:
	static constexpr int card_id = CARD_ID_FP1_002;

	// Haunted Creeper

	static void Deathrattle(GameEngine::Board & board, GameEngine::BoardObjects::MinionManipulator triggering_minion)
	{
		// summon (FP1_002t) * 2 when death
		Card card = CardDatabase::GetInstance().GetCard(CARD_ID_FP1_002t);

		auto inserter = triggering_minion.GetMinions().GetInserterBefore(board, triggering_minion.GetMinion());
#ifdef DEBUG
		if (inserter.IsEnd()) {
			std::cout << "deathrattle triggering minion is vanished!" << std::endl;
		}
#endif

		StageHelper::SummonMinion(board, card, inserter);
		StageHelper::SummonMinion(board, card, inserter);
	}

	static void AfterSummoned(GameEngine::BoardObjects::MinionManipulator & summoned_minion)
	{
		summoned_minion.AddOnDeathTrigger(Deathrattle);
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
