#pragma once

#include <stdexcept>

#include "game-engine/hook/listener.h"
#include "game-engine/enchantments/types.h"
#include "game-engine/enchantments/enchantment.h"
#include "game-engine/enchantments/owner.h"
#include "game-engine/player.h"

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
		virtual void AfterAdded(Minion & owner)
		{
			(void)owner;
		}

		virtual void BeforeRemoved(Minion & owner_)
		{
			if (this->minion_enchantments) {
				this->minion_enchantments->RemoveOwnedEnchantments();
			}
			if (this->player_enchantments) {
				this->player_enchantments->RemoveOwnedEnchantments();
			}
		}

	public:
		void AddEnchantment(Minion & minion, std::unique_ptr<Enchantment<Minion>> && enchantment)
		{
			using OwnerItem = EnchantmentTypes<Minion>::OwnerItem;

			auto managed_item = minion.enchantments.Add(std::move(enchantment));

			this->GetMinionEnchantmentsOwner().AddEnchantment(minion.enchantments, managed_item);
		}

		void AddEnchantment(Player & player, std::unique_ptr<Enchantment<Player>> && enchantment)
		{
			using OwnerItem = EnchantmentTypes<Player>::OwnerItem;

			auto managed_item = player.enchantments.Add(std::move(enchantment));
			
			this->GetPlayerEnchantmentsOwner().AddEnchantment(player.enchantments, managed_item);
		}

	protected:
		EnchantmentsOwner<Minion> & GetMinionEnchantmentsOwner()
		{
			if (!this->minion_enchantments) {
				this->minion_enchantments.reset(new EnchantmentsOwner<Minion>());
			}
			return *this->minion_enchantments;
		}

		EnchantmentsOwner<Player> & GetPlayerEnchantmentsOwner()
		{
			if (!this->player_enchantments) {
				this->player_enchantments.reset(new EnchantmentsOwner<Player>());
			}
			return *this->player_enchantments;
		}

	private:
		Minion & owner;
		std::unique_ptr<EnchantmentsOwner<Minion>> minion_enchantments;
		std::unique_ptr<EnchantmentsOwner<Player>> player_enchantments;
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