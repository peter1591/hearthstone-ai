#ifndef GAME_ENGINE_CARDS_CARD_CS2_222
#define GAME_ENGINE_CARDS_CARD_CS2_222

#include <stdexcept>
#include "card-base.h"

namespace GameEngine {
	namespace Cards {

		class Card_CS2_222
		{
		public:
			static constexpr int card_id = CARD_ID_CS2_222;

			// Stormwind Champion

			// TODO: add an convenient class for this kind of aura
			class Aura : public GameEngine::AuraToAllMinions
			{
			private: // hooks
				void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion)
				{
					if (this->CheckMinionShouldHaveAuraEnchantment(aura_owner, added_minion)) {
						this->AddAuraEnchantmentToMinion(added_minion);
					}
				}

			private:
				bool CheckMinionShouldHaveAuraEnchantment(Minion & aura_owner, Minion & minion)
				{
					// only add aura to friendly minions
					if (&aura_owner.GetMinions() != &minion.GetMinions()) return false;

					// only add aura to others
					if (&aura_owner.GetMinion() == &minion.GetMinion()) return false;

					return true;
				}

				void AddAuraEnchantmentToMinion(Minion & target_minion)
				{
					constexpr int attack_boost = 1;
					constexpr int hp_boost = 1;

					auto enchantment = std::make_unique<Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, 0, false>>();

					target_minion.AddEnchantment(std::move(enchantment), &this->enchantments_manager);
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

#endif
