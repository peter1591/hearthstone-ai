#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

	// return true if stage changed
	bool StageHelper::PlayMinion(Board & board, Card const& card, SlotIndex playing_side, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, board, playing_side, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		auto inserter = board.object_manager.GetMinionInserter(board, data.put_location);

		StageHelper::SummonMinion(card, inserter);

		return false;
	}

	bool StageHelper::SummonMinion(Card const & card, BoardObjects::MinionInserter & inserter)
	{
		BoardObjects::Minion summoning_minion;
		summoning_minion.Summon(card);

		if (inserter.GetMinions().IsFull()) return false;

		// add minion
		auto summoned_minion = inserter.InsertBefore(std::move(summoning_minion));
		
		Cards::CardCallbackManager::AfterSummoned(card.id, summoned_minion);

		return true;
	}

}