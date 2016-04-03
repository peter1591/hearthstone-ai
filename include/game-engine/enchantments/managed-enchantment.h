#pragma once

#include "enchantments.h"

namespace GameEngine
{
	template <typename Target>
	class ManagedEnchantment
	{
	public:
		typedef Enchantments<Target> Manager;
		typedef typename Manager::ManagedItem ManagedItem;

		friend class Manager; // only manager can manage underlying item

	private: // only accessible to Manager
		ManagedEnchantment(Enchantments<Target> & manager, ManagedItem const& item)
			: manager(manager), item(item)
		{
		}

	public:
		ManagedItem Get() const { return this->item; }

		void Remove() { this->manager.Remove(*this); }

	private:
		Manager & manager;
		ManagedItem item;
	};
} // namespace GameEngine