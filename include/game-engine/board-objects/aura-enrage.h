#pragma once

#include "aura.h"
#include "enchantments.h"
#include "minions-iterator.h"
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
			void AfterAdded(MinionManipulator & owner)
			{
				if (aura_owner->GetHP() < aura_owner->GetMaxHP())
				{
					// enraged
				}
			}

			void BeforeRemoved(MinionManipulator & owner)
			{
				// Note: there should be no way to remove the enrage mechanism without removing the enrage enhancement
				this->enchantments_manager.RemoveOwnedEnchantments(owner);
			}

			virtual void HookMinionEnraged(MinionManipulator &aura_owner, MinionManipulator & enraged_minion) = 0;
			virtual void HookMinionUnEnraged(MinionManipulator &aura_owner, MinionManipulator & unenraged_minion) = 0;

		protected:
			GameEngine::BoardObjects::EnchantmentOwner enchantments_manager;
		};

	} // namespace BoardObjects
} // namespace GameEngine