#ifndef GAME_ENGINE_CARDS_CARD_AT_117
#define GAME_ENGINE_CARDS_CARD_AT_117

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_AT_117
		{
		public:
			static constexpr int card_id = CARD_ID_AT_117;

			// Master of Ceremonies
			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				if (summoned_minion.GetMinions().GetPlayer().GetTotalSpellDamage() > 0) {
					summoned_minion->enchantments.Add(std::make_unique<Enchantment_BuffMinion_C<2, 2, 0, 0, false>>());
				}
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
