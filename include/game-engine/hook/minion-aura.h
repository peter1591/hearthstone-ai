#pragma once

#include <stdexcept>

#include "game-engine/hook/listener.h"
#include "game-engine/enchantments/owner.h"

namespace GameEngine
{
	class Minion;

	// An aura (i.e., hook listener) hold by a minion
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

			if (this->minion_enchantments) {
				this->minion_enchantments->RemoveOwnedEnchantments();
			}
		}

	public:
		EnchantmentOwner<Minion> & GetMinionEnchantmentsOwner()
		{
			if (!this->minion_enchantments) {
				this->minion_enchantments.reset(new EnchantmentOwner<Minion>());
			}
			return *this->minion_enchantments;
		}

	private:
		Minion & owner;
		std::unique_ptr<EnchantmentOwner<Minion>> minion_enchantments;
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