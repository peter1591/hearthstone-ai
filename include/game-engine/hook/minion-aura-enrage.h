#pragma once

#include "game-engine/board-objects/minion.h"
#include "game-engine/hook/minion-aura.h"

namespace GameEngine {
	// This class add enchantments to all minions
	// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
	class AuraEnrage : public MinionAura
	{
	public:
		AuraEnrage(Minion & owner) : MinionAura(owner), enraged(false) {}
		virtual ~AuraEnrage() {}

	public: // hooks
		void AfterAdded()
		{
			if (this->GetOwner().GetHP() < this->GetOwner().GetMaxHP()) {
				this->HookAfterOwnerEnraged(this->GetOwner());
			}
			else {
				this->HookAfterOwnerUnEnraged(this->GetOwner());
			}
		}

		void HookAfterOwnerEnraged(Minion & aura_owner) // TODO: check parameter
		{
			MinionAura::HookAfterOwnerEnraged();

			if (enraged) return;

			this->AddEnrageEnchantment(aura_owner);
			enraged = true;
		}

		void HookAfterOwnerUnEnraged(Minion & aura_owner)
		{
			(void)aura_owner;

			MinionAura::HookAfterOwnerUnEnraged();

			if (!enraged) return;

			this->GetMinionEnchantmentsOwner().RemoveOwnedEnchantments();
			enraged = false;
		}

		virtual void AddEnrageEnchantment(Minion & aura_owner) = 0;

	private:
		bool enraged;
	};

} // namespace GameEngine