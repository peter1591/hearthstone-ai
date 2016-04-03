#pragma once

#include "game-engine/board.h"
#include "game-engine/enchantments/enchantments.h"
#include "game-engine/hook/listener.h"

namespace GameEngine {
	// This class add enchantments to all minions
	// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
	class AuraToAllMinions : public HookListener
	{
	public:
		AuraToAllMinions() {}
		virtual ~AuraToAllMinions() {}

	protected: // hooks
		void AfterAdded(Minion & aura_owner)
		{
			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->HookAfterMinionAdded(aura_owner, *it);
			}
			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->HookAfterMinionAdded(aura_owner, *it);
			}
		}

		void BeforeRemoved(Minion & owner)
		{
			this->enchantments_manager.RemoveOwnedEnchantments();
		}

		virtual void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion) = 0;

	protected:
		EnchantmentOwner<Minion> enchantments_manager;
	};

} // namespace GameEngine