#pragma once

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_029
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_029;

			// Leper Gnome
			
			static void Deathrattle(GameEngine::Board & board, GameEngine::BoardObjects::MinionsIteratorWithIndex triggering_minion)
			{
				// deal 2 damage to opponent hero
				constexpr int damage = 2;
				
				if (SlotIndexHelper::IsPlayerSide(triggering_minion.GetSlotIdx()))
				{
					StageHelper::DealDamage(board, board.object_manager.GetOpponentHero(), damage);
				}
				else {
					StageHelper::DealDamage(board, board.object_manager.GetPlayerHero(), damage);
				}
			}

			static void AfterSummoned(GameEngine::Board &, GameEngine::BoardObjects::MinionsIteratorWithIndex & summoned_minion)
			{
				summoned_minion->AddOnDeathTrigger(Deathrattle);
			}
		};

	} // namespace Cards
} // namespace GameEngine
