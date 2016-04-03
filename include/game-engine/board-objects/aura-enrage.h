#pragma once

#include "game-engine/board.h"
#include "game-engine/enchantments/enchantments.h"
#include "game-engine/enchantments/owner.h"
#include "game-engine/hook/listener.h"

namespace GameEngine {
	// This class add enchantments to all minions
	// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
	class AuraEnrage : public HookListener
	{
	public:
		AuraEnrage() {}
		virtual ~AuraEnrage() {}

	protected: // hooks
		void AfterAdded(Minion & aura_owner)
		{
			if (aura_owner.GetHP() < aura_owner.GetMaxHP()) {
				this->HookAfterOwnerEnraged(aura_owner);
			}
			else {
				this->HookAfterOwnerUnEnraged(aura_owner);
			}
		}

		void BeforeRemoved(Minion & owner)
		{
			this->enchantments_manager.RemoveOwnedEnchantments();
		}

		void HookAfterOwnerEnraged(Minion & aura_owner) {
			if (this->enchantments_manager.IsEmpty()) this->AddEnrageEnchantment(aura_owner);
		}

		void HookAfterOwnerUnEnraged(Minion & aura_owner) {
			this->enchantments_manager.RemoveOwnedEnchantments();
		}

		virtual void AddEnrageEnchantment(Minion & aura_owner) = 0;

	protected:
		GameEngine::EnchantmentOwner<Minion> enchantments_manager;
	};

} // namespace GameEngine