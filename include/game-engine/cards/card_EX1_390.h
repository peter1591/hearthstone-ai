#pragma once

#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_390
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_390;

			// Tauren Warrior

			class Aura : public GameEngine::AuraEnrage
			{
			public:
				Aura(GameEngine::Minion & owner) : AuraEnrage(owner) {}

			private:
				void AddEnrageEnchantment(GameEngine::Minion & aura_owner)
				{
					constexpr int attack_boost = 3;
					constexpr int hp_boost = 0;

					auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, 0, false>>();

					aura_owner.enchantments.Add(std::move(enchantment), &this->enchantments_manager);
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
