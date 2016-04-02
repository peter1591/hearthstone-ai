#ifndef GAME_ENGINE_CARDS_CARD_GVG_112
#define GAME_ENGINE_CARDS_CARD_GVG_112

#include <stdexcept>
#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_GVG_112
		{
		public:
			static constexpr int card_id = CARD_ID_GVG_112;

			// Mogor the Ogre

			class Aura : public GameEngine::BoardObjects::AuraToAllMinions
			{
			private: // hooks
				void HookAfterMinionAdded(BoardObjects::Minion & aura_owner, BoardObjects::Minion & added_minion)
				{
					this->AddAuraEnchantmentToMinion(added_minion);
				}

			private:
				void AddAuraEnchantmentToMinion(BoardObjects::Minion & target_minion)
				{
					constexpr int buff_stat = 1 << BoardObjects::MinionStat::FLAG_FORGETFUL;

					auto enchantment = std::make_unique<BoardObjects::Enchantment_BuffMinion_C<0, 0, buff_stat, false>>();

					target_minion.AddEnchantment(std::move(enchantment), &this->enchantments_manager);
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
					std::size_t result = std::hash<int>()(Card_GVG_112::card_id);

					return result;
				}
			};

			static void AfterSummoned(GameEngine::BoardObjects::MinionIterator summoned_minion)
			{
				summoned_minion->AddAura(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
