#include "game-engine/stages/helper.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

	// return true if stage changed
	bool StageHelper::PlayMinion(Board & board, Card const& card, PlayMinionData const & data)
	{
		Cards::CardCallbackManager::BattleCry(card.id, board, data);
		if (StageHelper::CheckHeroMinionDead(board)) return true;

		StageHelper::SummonMinion(board, card, data);

		return false;
	}

	bool StageHelper::SummonMinion(Board & board, Card const & card, PlayMinionData const & data)
	{
		Minion minion;
		minion.Summon(card);

		if (Targetor::IsPlayerSide(data.put_location))
		{
			if (board.player_minions.IsFull()) return false;

			int idx = data.put_location - Targetor::GetPlayerMinionIndex(0);
			Minion & summoning_minion = board.player_minions.AddMinion(minion, idx);
			Cards::CardCallbackManager::OnSummon(card.id, board, Targetor::GetPlayerHeroIndex(), data.put_location, summoning_minion);
		}
		else {
			if (board.opponent_minions.IsFull()) return false;

			int idx = data.put_location - Targetor::GetOpponentMinionIndex(0);
			Minion & summoning_minion = board.opponent_minions.AddMinion(minion, idx);
			Cards::CardCallbackManager::OnSummon(card.id, board, Targetor::GetOpponentHeroIndex(), data.put_location, summoning_minion);
		}

		return true;
	}

}