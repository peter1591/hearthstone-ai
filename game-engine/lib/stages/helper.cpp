#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

	// return true if stage changed
	bool StageHelper::PlayMinion(Board & board, Card const& card, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, board, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		StageHelper::SummonMinion(board, card, data.put_location);

		return false;
	}

	bool StageHelper::SummonMinion(Board & board, Card const & card, SlotIndex location)
	{
		BoardObjects::Minion minion;
		minion.Summon(card);

		if (SlotIndexHelper::IsPlayerSide(location))
		{
			if (board.object_manager.IsPlayerMinionsFull()) return false;

			BoardObjects::Minion & summoning_minion = board.object_manager.AddMinion(location, minion);
			Cards::CardCallbackManager::OnSummon(card.id, board, SLOT_PLAYER_SIDE, location, summoning_minion);
		}
		else {
			if (board.object_manager.IsOpponentMinionsFull()) return false;

			BoardObjects::Minion & summoning_minion = board.object_manager.AddMinion(location, minion);
			Cards::CardCallbackManager::OnSummon(card.id, board, SLOT_OPPONENT_SIDE, location, summoning_minion);
		}

		return true;
	}

}