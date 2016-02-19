#ifndef GAME_ENGINE_CARDS_CARD_CS2_222
#define GAME_ENGINE_CARDS_CARD_CS2_222

#include <stdexcept>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/common.h"
#include "game-engine/board-objects/aura-to-all-minions.h"
#include "game-engine/board-objects/enchantment.h"
#include "game-engine/board-objects/minion-manipulator.h"

namespace GameEngine {
	namespace Cards {

		class Card_CS2_222
		{
		public:
			static constexpr int card_id = CARD_ID_CS2_222;

			// Stormwind Champion

			class Aura : public GameEngine::BoardObjects::AuraToAllMinions
			{
			private: // hooks
				void HookAfterMinionAdded(BoardObjects::MinionManipulator & aura_owner, BoardObjects::MinionManipulator & added_minion)
				{
					if (this->CheckMinionShouldHaveAuraEnchantment(aura_owner, added_minion)) {
						this->AddAuraEnchantmentToMinion(added_minion);
					}
				}

			private:
				bool CheckMinionShouldHaveAuraEnchantment(BoardObjects::MinionManipulator & aura_owner, BoardObjects::MinionManipulator & minion)
				{
					// only add aura to friendly minions
					if (&aura_owner.GetMinions() != &minion.GetMinions()) return false;

					// only add aura to others
					if (&aura_owner.GetMinion() == &minion.GetMinion()) return false;

					return true;
				}

				void AddAuraEnchantmentToMinion(BoardObjects::MinionManipulator & target_minion)
				{
					constexpr int attack_boost = 1;
					constexpr int hp_boost = 1;

					auto enchantment = std::make_unique<BoardObjects::Enchantment_BuffMinion<attack_boost, hp_boost, 0, false>>();

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
					std::size_t result = std::hash<int>()(card_id);

					return result;
				}
			};

			static void AfterSummoned(GameEngine::BoardObjects::MinionManipulator & summoned_minion)
			{
				summoned_minion.AddAura(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
