#ifndef GAME_ENGINE_CARDS_CARD_BRM_019
#define GAME_ENGINE_CARDS_CARD_BRM_019

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/enchantment.h"
#include "game-engine/board-objects/aura.h"

namespace GameEngine {
	namespace Cards {

		class Card_BRM_019
		{
		public:
			static constexpr int card_id = CARD_ID_BRM_019;

			// Grim Patron

			class Aura : public GameEngine::BoardObjects::Aura
			{
			public:
				Aura() : owner(nullptr) {}

				void AfterAdded(GameEngine::BoardObjects::Minion & owner) { this->owner = &owner; }

				void HookAfterMinionDamaged(GameEngine::BoardObjects::Minions & minions, GameEngine::BoardObjects::Minion & minion, int damage) 
				{
					if (&minion != this->owner) return;

					if (minion.GetMinion().stat.GetHP() <= 0) return; // not survives the damage

					auto it_owner = minions.GetIteratorForSpecificMinion(*this->owner);
					if (it_owner.IsEnd()) throw std::runtime_error("owner vanished");
					Card card = CardDatabase::GetInstance().GetCard(CARD_ID_BRM_019);
					it_owner.GoToNext(); // summon the new patron to the right
					StageHelper::SummonMinion(card, it_owner);
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

			private:
				GameEngine::BoardObjects::Minion * owner;
			};

			static void AfterSummoned(GameEngine::BoardObjects::Minion & summoned_minion)
			{
				summoned_minion.AddAura(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
