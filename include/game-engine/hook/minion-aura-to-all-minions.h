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
			MinionAura::AfterAdded(aura_owner);

			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->HookAfterMinionAdded(aura_owner, *it);
			}
			for (auto it = aura_owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
				this->HookAfterMinionAdded(aura_owner, *it);
			}
		}
	};

} // namespace GameEngine