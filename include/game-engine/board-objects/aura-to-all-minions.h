#pragma once

#include "aura.h"
#include "enchantments.h"
#include "minions-iterator.h"
#include "game-engine/board.h"

namespace GameEngine {
	namespace BoardObjects {

		// This class add enchantments to all minions
		// and will invoke the hook 'HookAfterMinionAdded' for every existing minions
		class AuraToAllMinions : public Aura
		{
		public:
			AuraToAllMinions() {}
			virtual ~AuraToAllMinions() {}

		protected: // hooks
			void AfterAdded(MinionManipulator & aura_owner)
			{
				for (auto it = aura_owner.board->object_manager.GetMinionInserterAtBeginOfSide(*aura_owner.board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
					this->HookAfterMinionAdded(aura_owner, it.ConverToManipulator());
				}
				for (auto it = aura_owner.board->object_manager.GetMinionInserterAtBeginOfSide(*aura_owner.board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
					this->HookAfterMinionAdded(aura_owner, it.ConverToManipulator());
				}
			}

			void BeforeRemoved(MinionManipulator & owner)
			{
				this->enchantments_manager.RemoveOwnedEnchantments(owner);
			}

			virtual void HookAfterMinionAdded(MinionManipulator & aura_owner, MinionManipulator & added_minion) = 0;

		protected:
			GameEngine::BoardObjects::EnchantmentOwner enchantments_manager;
		};

	} // namespace BoardObjects
} // namespace GameEngine