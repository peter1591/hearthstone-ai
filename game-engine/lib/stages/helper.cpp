#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

	// return true if stage changed
	bool StageHelper::PlayMinion(Board & board, Card const& card, SlotIndex playing_side, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, board, playing_side, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		auto location = board.object_manager.GetMinionIteratorWithIndex(data.put_location);
		StageHelper::SummonMinion(board, card, location);

		return false;
	}

	bool StageHelper::SummonMinion(Board & board, Card const & card, BoardObjects::MinionsIteratorWithIndex location)
	{
		BoardObjects::Minion summoning_minion;
		summoning_minion.Summon(card);

		auto & minions = location.GetOwner();

		if (minions.IsFull()) return false;

		// add minion
		auto summoned_minion = location.InsertBefore(std::move(summoning_minion));
		
		Cards::CardCallbackManager::AfterSummoned(card.id, board, summoned_minion);

#ifdef DEBUG
		if (&board.object_manager.GetMinionIteratorWithIndex(summoned_minion.GetSlotIdx()).GetOwner() != &summoned_minion.GetOwner())
		{
			throw std::runtime_error("owner's slot index does not match");
		}
#endif

		board.object_manager.HookAfterMinionAdded(board, summoned_minion);

		return true;
	}

}