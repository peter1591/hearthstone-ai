#ifndef GAME_ENGINE_CARDS_CARD_CS2_222
#define GAME_ENGINE_CARDS_CARD_CS2_222

#include <stdexcept>
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/common.h"
#include "game-engine/board-objects/aura.h"
#include "game-engine/board-objects/enchantment.h"

namespace GameEngine {
	namespace Cards {

		class Card_CS2_222
		{
		public:
			static constexpr int card_id = CARD_ID_CS2_222;

			// Stormwind Champion

			class Aura : public GameEngine::BoardObjects::Aura
			{
			private: // hooks
				void AfterAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner)
				{
					// add aura effect to existing minions
					
					SlotIndex side = SlotIndexHelper::GetSide(aura_owner.GetSlotIdx());

#ifdef DEBUG
					if (&board.object_manager.GetMinionIteratorWithIndex(side).GetOwner() != &aura_owner.GetOwner())
					{
						throw std::runtime_error("owner's slot index does not match");
					}
#endif

					for (auto it_friendly_minions = board.object_manager.GetMinionIteratorWithIndex(side); !it_friendly_minions.IsEnd(); it_friendly_minions.GoToNext())
					{
						if (this->CheckMinionShouldHaveAuraEnchantment(aura_owner, it_friendly_minions)) {
							this->AddAuraEnchantmentToMinion(*it_friendly_minions);
						}
					}
				}

				void BeforeRemoved()
				{
					this->enchantments_manager.RemoveOwnedEnchantments();
				}

				void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, BoardObjects::MinionsIteratorWithIndex &minion)
				{
					if (this->CheckMinionShouldHaveAuraEnchantment(aura_owner, minion)) {
						this->AddAuraEnchantmentToMinion(*minion);
					}
				}

			private:
				bool CheckMinionShouldHaveAuraEnchantment(BoardObjects::MinionsIteratorWithIndex &aura_owner, BoardObjects::MinionsIteratorWithIndex const& minion)
				{
					// only add aura to friendly minions
					if (&minion.GetOwner() != &aura_owner.GetOwner()) return false;

					// only add aura to others
					if (&(*minion) == &(*aura_owner)) return false;

					return true;
				}

				void AddAuraEnchantmentToMinion(BoardObjects::Minion &target_minion)
				{
					constexpr int attack_boost = 1;
					constexpr int hp_boost = 1;

					auto * enchantment = new BoardObjects::Enchantment_AttackHPBoost<attack_boost, hp_boost, false>();

					target_minion.AddEnchantment(enchantment, &this->enchantments_manager);
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
				GameEngine::BoardObjects::EnchantmentOwner enchantments_manager;
			};

			static void AfterSummoned(GameEngine::Board & board, GameEngine::BoardObjects::MinionsIteratorWithIndex & summoned_minion)
			{
				summoned_minion.AddAura(board, new Aura());
			}
		};

	} // namespace Cards
} // namespace GameEngine

#endif
