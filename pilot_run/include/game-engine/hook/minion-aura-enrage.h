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
				this->HookAfterOwnerEnraged();
			}
			else {
				this->HookAfterOwnerUnEnraged();
			}
		}

		void HookAfterOwnerEnraged()
		{
			MinionAura::HookAfterOwnerEnraged();

			if (enraged) return;

			this->AddEnrageEnchantment();
			enraged = true;
		}

		void HookAfterOwnerUnEnraged()
		{
			MinionAura::HookAfterOwnerUnEnraged();

			if (!enraged) return;

			this->GetMinionEnchantmentsOwner().RemoveOwnedEnchantments();
			enraged = false;
		}

		virtual void AddEnrageEnchantment() = 0;

	private:
		bool enraged;
	};

} // namespace GameEngine