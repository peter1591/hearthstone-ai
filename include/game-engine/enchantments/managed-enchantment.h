#pragma once

#include "enchantments.h"

namespace GameEngine
{
	// This is a wrapper class for enchantments,
	// which has a reference to the enchantments manager
	// so the caller can call Remove() without parameter

	// This class also acts as a token to access the removal function in enchantments
	//    See: Enchantments::Remove()

	template <typename Target>
	class ManagedEnchantment
	{
	public:
		typedef Enchantments<Target> Manager;
		typedef typename Manager::ManagedItem ManagedItem;

		friend class Manager; // only manager can manage underlying item

		bool operator==(ManagedEnchantment<Target> const& rhs) const = delete;
		bool operator!=(ManagedEnchantment<Target> const& rhs) const = delete;

		bool EqualsTo(ManagedItem const& rhs) const { return this->item == rhs; }

	private: // only accessible to Manager
		ManagedEnchantment(Enchantments<Target> & manager, ManagedItem const& item)
			: manager(manager), item(item)
		{
		}

		ManagedItem Get() const { return this->item; }

	public:
		void Remove() { this->manager.Remove(*this); }

	private:
		Manager & manager;
		ManagedItem item;
	};
} // namespace GameEngine