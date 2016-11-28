#pragma once

#include "game-engine/board.h"
#include "game-engine/enchantments/enchantments.h"
#include "game-engine/hook/minion-aura.h"

namespace GameEngine {
	// This class add enchantments to all minions
	// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
	class AuraToAllMinions : public MinionAura
	{
	public:
		AuraToAllMinions(Minion & owner) : MinionAura(owner) {}
		virtual ~AuraToAllMinions() {}

	public: // hooks
		void AfterAdded()
		{
			MinionAura::AfterAdded();

			for (auto it = this->GetOwner().GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->Invoke(*it);
			}
			for (auto it = this->GetOwner().GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->Invoke(*it);
			}
		}

		void HookAfterMinionAdded(Minion & added_minion)
		{
			MinionAura::HookAfterMinionAdded(added_minion);
			this->Invoke(added_minion);
		}

		void HookAfterMinionTransformed(Minion & minion)
		{
			MinionAura::HookAfterMinionTransformed(minion);
			this->Invoke(minion);
		}

		// default: apply to all minions
		virtual bool CheckMinionShouldHaveAuraEnchantment(Minion & minion) { (void)minion; return true; }
		virtual void AddAuraEnchantmentToMinion(Minion & target_minion) = 0;

	private:
		void Invoke(Minion & minion)
		{
			if (this->CheckMinionShouldHaveAuraEnchantment(minion)) {
				this->AddAuraEnchantmentToMinion(minion);
			}
		}
	};

} // namespace GameEngine