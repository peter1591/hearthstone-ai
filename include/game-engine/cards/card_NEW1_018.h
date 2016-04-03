#ifndef GAME_ENGINE_CARDS_CARD_NEW1_018
#define GAME_ENGINE_CARDS_CARD_NEW1_018

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_NEW1_018
		{
		public:
			static constexpr int card_id = CARD_ID_NEW1_018;

			// Bloodsail Raider

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				Hero * playing_hero = nullptr;

				if (summoned_minion->IsPlayerSide()) playing_hero = &summoned_minion.GetBoard().player.hero;
				else playing_hero = &summoned_minion.GetBoard().opponent.hero;

				const int attack_boost = playing_hero->GetWeaponAttack();

				if (attack_boost > 0) {
					auto enchant = std::make_unique<Enchantment_BuffMinion>(attack_boost, 0, 0, 0, false);
					summoned_minion->enchantments.Add(std::move(enchant), nullptr);
				}
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
