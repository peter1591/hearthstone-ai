#pragma once

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion-manipulator.h"
#include "game-engine/board-objects/aura-enrage.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_412
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_412;

			// Raging Worgen

			class Aura : public GameEngine::BoardObjects::AuraEnrage
			{
			private:
				void AddEnrageEnchantment(GameEngine::BoardObjects::MinionManipulator const& aura_owner)
				{
					constexpr int attack_boost = 1;
					constexpr int hp_boost = 0;
					constexpr int buff_stat = 1 << BoardObjects::MinionStat::FLAG_WINDFURY;

					auto * enchantment = new BoardObjects::Enchantment_BuffMinion<attack_boost, hp_boost, buff_stat, false>();

					aura_owner.AddEnchantment(enchantment, &this->enchantments_manager);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::BoardObjects::Aura const& rhs_base) const
				{
					auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
					if (!rhs) return false;

					return true;
				}

				std::size_t GetHash() const
				{
					std::size_t result = std::hash<int>()(card_id);

					return result;
				}
			};

			static void AfterSummoned(GameEngine::BoardObjects::MinionManipulator const& summoned_minion)
			{
				summoned_minion.AddAura(new Aura());
			}
		};

	} // namespace Cards
} // namespace GameEngine
