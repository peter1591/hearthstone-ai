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

		void HookAfterOwnerEnraged(Minion & aura_owner)
		{
			MinionAura::HookAfterOwnerEnraged(aura_owner);

			if (enraged) return;

			this->AddEnrageEnchantment(aura_owner);
			enraged = true;
		}

		void HookAfterOwnerUnEnraged(Minion & aura_owner)
		{
			MinionAura::HookAfterOwnerUnEnraged(aura_owner);

			if (!enraged) return;

			this->minion_enchantments.RemoveOwnedEnchantments();
			enraged = false;
		}

		virtual void AddEnrageEnchantment(Minion & aura_owner) = 0;

	private:
		bool enraged;
	};

} // namespace GameEngine