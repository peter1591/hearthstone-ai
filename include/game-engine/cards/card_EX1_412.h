#pragma once

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_412
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_412;

			// Raging Worgen

			class Aura : public GameEngine::AuraEnrage
			{
			private:
				void AddEnrageEnchantment(GameEngine::Minion & aura_owner)
				{
					constexpr int attack_boost = 1;
					constexpr int hp_boost = 0;
					constexpr int buff_stat = 1 << MinionStat::FLAG_WINDFURY;

					auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, buff_stat, false>>();

					aura_owner.AddEnchantment(std::move(enchantment), &this->enchantments_manager);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::HookListener const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base); }
				std::size_t GetHash() const { return typeid(*this).hash_code(); }
			};

			static void AfterSummoned(GameEngine::MinionIterator summoned_minion)
			{
				summoned_minion->AddHookListener(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine
