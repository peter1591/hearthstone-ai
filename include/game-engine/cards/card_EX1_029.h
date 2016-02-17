#pragma once

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion-manipulator.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_029
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_029;

			// Leper Gnome
			
			static void Deathrattle(GameEngine::BoardObjects::MinionManipulator triggering_minion)
			{
				// deal 2 damage to opponent hero
				constexpr int damage = 2;

				auto & board = triggering_minion.GetBoard();

				if (triggering_minion.IsPlayerSide()) {
					StageHelper::DealDamage(board.object_manager.GetObject(board, SLOT_OPPONENT_HERO), damage, false);
				}
				else {
					StageHelper::DealDamage(board.object_manager.GetObject(board, SLOT_PLAYER_HERO), damage, false);
				}
			}

			static void AfterSummoned(GameEngine::BoardObjects::MinionManipulator & summoned_minion)
			{
				summoned_minion.AddOnDeathTrigger(Deathrattle);
			}
		};

	} // namespace Cards
} // namespace GameEngine
