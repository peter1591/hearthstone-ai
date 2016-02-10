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
			void AfterAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner)
			{
				for (auto it = board.object_manager.GetPlayerMinionsIteratorWithIndex(); !it.IsEnd(); it.GoToNext()) {
					HookAfterMinionAdded(board, aura_owner, it);
				}
			}

			void BeforeRemoved()
			{
				this->enchantments_manager.RemoveOwnedEnchantments();
			}

			virtual void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, MinionsIteratorWithIndex & added_minion) = 0;

		protected:
			GameEngine::BoardObjects::EnchantmentOwner enchantments_manager;
		};

	} // namespace BoardObjects
} // namespace GameEngine