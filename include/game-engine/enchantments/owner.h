#pragma once

#include <list>
#include "enchantment.h"

namespace GameEngine {
	namespace BoardObjects {
		class EnchantmentOwner
		{
		public:
			bool IsEmpty() const;

			void RemoveOwnedEnchantments(GameEngine::BoardObjects::Minion & owner);

			// hooks
			void EnchantmentAdded(Enchantment<Minion> * enchantment);
			void EnchantmentRemoved(Enchantment<Minion> * enchantment);

		private:
			std::list<Enchantment<Minion> *> minion_enchantments;
		};
	}
}