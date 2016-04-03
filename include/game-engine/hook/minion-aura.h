#pragma once

#include <stdexcept>

#include "game-engine/hook/listener.h"
#include "game-engine/enchantments/owner.h"

namespace GameEngine
{
	class Minion;

	// An aura hold by a minion
	// An aura is actually a hook listener
	class MinionAura : public HookListener
	{
		friend std::hash<MinionAura>;

	public:
		MinionAura(Minion & owner) : owner(owner) {}
		~MinionAura() {}

		Minion & GetOwner() const { return this->owner; }

	public:
		void BeforeRemoved(Minion & owner)
		{
			HookListener::BeforeRemoved(owner);
			this->enchantments_manager.RemoveOwnedEnchantments();
		}

	protected:
		EnchantmentOwner<Minion> enchantments_manager;

	private:
		Minion & owner;
	};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::MinionAura> {
		typedef GameEngine::MinionAura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}