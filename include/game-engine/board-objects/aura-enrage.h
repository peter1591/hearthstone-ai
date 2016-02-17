#pragma once

#include "aura.h"
#include "enchantments.h"
#include "game-engine/board.h"

namespace GameEngine {
	namespace BoardObjects {

		// This class add enchantments to all minions
		// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
		class AuraEnrage : public Aura
		{
		public:
			AuraEnrage() {}
			virtual ~AuraEnrage() {}

		protected: // hooks
			void AfterAdded(MinionManipulator const& aura_owner)
			{
				if (aura_owner.GetHP() < aura_owner.GetMaxHP()) {
					this->HookAfterOwnerEnraged(aura_owner);
				}
				else {
					this->HookAfterOwnerUnEnraged(aura_owner);
				}
			}

			void BeforeRemoved(MinionManipulator const& owner)
			{
				// Note: there should be no way to remove the enrage mechanism without removing the enrage enhancement
				this->enchantments_manager.RemoveOwnedEnchantments(owner);
			}

			void HookAfterOwnerEnraged(MinionManipulator const& aura_owner) {
				if (this->enchantments_manager.IsEmpty()) this->AddEnrageEnchantment(aura_owner);
			}

			void HookAfterOwnerUnEnraged(MinionManipulator const& aura_owner) {
				this->enchantments_manager.RemoveOwnedEnchantments(aura_owner);
			}

			virtual void AddEnrageEnchantment(MinionManipulator const& aura_owner) = 0;

		protected:
			GameEngine::BoardObjects::EnchantmentOwner enchantments_manager;
		};

	} // namespace BoardObjects
} // namespace GameEngine