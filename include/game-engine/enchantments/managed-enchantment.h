#pragma once

#include "enchantments.h"

namespace GameEngine
{
	template <typename Target>
	class ManagedEnchantment
	{
	public:
		typedef Enchantments<Target> Manager;
		typedef typename Manager::container_type::ManagedItem ManagedItem;

		friend class Manager; // only manager can manage underlying item

	public:
		ManagedEnchantment(Enchantments<Target> & manager, ManagedItem const& item)
			: manager(manager), item(item)
		{
		}

		ManagedItem const& Get() const { return this->item; }

		void Remove() { this->manager.Remove(*this); }

	private: // only accessible to Manager
		ManagedItem & Get() { return this->item; }

	private:
		Manager & manager;
		ManagedItem item;
	};
} // namespace GameEngine