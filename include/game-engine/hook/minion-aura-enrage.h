#pragma once

#include "game-engine/board-objects/minion.h"
#include "game-engine/hook/minion-aura.h"

namespace GameEngine {
	// This class add enchantments to all minions
	// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
	class AuraEnrage : public MinionAura
	{
	public:
		AuraEnrage(Minion & owner) : MinionAura(owner) {}
		virtual ~AuraEnrage() {}

	public: // hooks
		void AfterAdded(Minion & aura_owner)
		{
			MinionAura::AfterAdded(aura_owner);

			if (aura_owner.GetHP() < aura_owner.GetMaxHP()) {
				this->HookAfterOwnerEnraged(aura_owner);
			}
			else {
				this->HookAfterOwnerUnEnraged(aura_owner);
			}
		}

		void HookAfterOwnerEnraged(Minion & aura_owner) {
			MinionAura::HookAfterOwnerEnraged(aura_owner);

			if (this->enchantments_manager.IsEmpty()) this->AddEnrageEnchantment(aura_owner);
		}

		void HookAfterOwnerUnEnraged(Minion & aura_owner) {
			MinionAura::HookAfterOwnerUnEnraged(aura_owner);

			this->enchantments_manager.RemoveOwnedEnchantments();
		}

		virtual void AddEnrageEnchantment(Minion & aura_owner) = 0;
	};

} // namespace GameEngine