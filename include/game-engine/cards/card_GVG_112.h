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

			class Aura : public GameEngine::AuraToAllMinions
			{
			public:
				Aura(Minion & owner) : AuraToAllMinions(owner) {}

			private: // hooks
				void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion)
				{
					constexpr int buff_stat = 1 << MinionStat::FLAG_FORGETFUL;

					auto enchantment = std::make_unique<Enchantment_BuffMinion_C<0, 0, 0, buff_stat, false>>();

					added_minion.enchantments.Add(std::move(enchantment), &this->minion_enchantments);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base); }
				std::size_t GetHash() const { return typeid(*this).hash_code(); }
			};

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				summoned_minion->auras.Add<Aura>();
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
