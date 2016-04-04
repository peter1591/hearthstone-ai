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
		void AfterAdded(Minion & aura_owner)
		{
#ifdef DEBUG
			if (&aura_owner != &this->GetOwner()) throw std::runtime_error("aura owner should be the same with the one actually added this aura");
#endif

			MinionAura::AfterAdded(aura_owner);

			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->Invoke(*it);
			}
			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->Invoke(*it);
			}
		}

		void HookAfterMinionAdded(Minion & added_minion)
		{
			MinionAura::HookAfterMinionAdded(added_minion);
			this->Invoke(added_minion);
		}

		// default: apply to all minions
		virtual bool CheckMinionShouldHaveAuraEnchantment(Minion & minion) { return true; }
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