#ifndef GAME_ENGINE_CARDS_CARD_BRM_019
#define GAME_ENGINE_CARDS_CARD_BRM_019

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_BRM_019
		{
		public:
			static constexpr int card_id = CARD_ID_BRM_019;

			// Grim Patron

			class Aura : public GameEngine::Aura
			{
			public:
				Aura() : owner(nullptr) {}

				void AfterAdded(GameEngine::Minion & owner) { this->owner = &owner; }

				void HookAfterMinionDamaged(GameEngine::Minion & minion, int damage) 
				{
					if (&minion != this->owner) return;

					if (minion.GetMinion().stat.GetHP() <= 0) return; // not survives the damage

					auto it_owner = minion.GetMinions().GetIteratorForSpecificMinion(*this->owner);
					if (it_owner.IsEnd()) throw std::runtime_error("owner vanished");
					Card card = CardDatabase::GetInstance().GetCard(Card_BRM_019::card_id);
					it_owner.GoToNext(); // summon the new patron to the right
					StageHelper::SummonMinion(card, it_owner);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::Aura const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base); }
				std::size_t GetHash() const { return typeid(*this).hash_code(); }

			private:
				GameEngine::Minion * owner;
			};

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				summoned_minion->AddAura(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
