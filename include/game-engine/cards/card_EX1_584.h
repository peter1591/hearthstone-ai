#pragma once

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_584
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_584;

			// Ancient Mage
			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				typedef Enchantment_BuffMinion_C<0, 0, 1, 0, false> EnchantType;

				if (summoned_minion.IsBegin() == false) {
					auto prev = summoned_minion;
					prev.GoToPrevious()->AddEnchantment(std::make_unique<EnchantType>(), nullptr);
				}

				auto next = summoned_minion;
				next.GoToNext();
				if (next.IsEnd() == false) next->AddEnchantment(std::make_unique<EnchantType>(), nullptr);
			}
		};

	} // namespace Cards
} // namespace GameEngine
