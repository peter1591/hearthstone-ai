#pragma once

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_029
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_029;

			// Leper Gnome
			
			static void Deathrattle(GameEngine::MinionIterator & triggering_minion)
			{
				// deal 2 damage to opponent hero
				constexpr int damage = 2;

				auto & board = triggering_minion.GetBoard();

				if (triggering_minion.IsPlayerSide()) {
					StageHelper::DealDamage(board.opponent.hero, damage, false);
				}
				else {
					StageHelper::DealDamage(board.player.hero, damage, false);
				}
			}

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				summoned_minion->AddOnDeathTrigger(GameEngine::Minion::OnDeathTrigger(Deathrattle));
			}
		};

	} // namespace Cards
} // namespace GameEngine
