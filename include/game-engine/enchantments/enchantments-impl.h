#include "game-engine/enchantments/enchantments.h"
#include "game-engine/enchantments/enchantment.h"
#include "game-engine/enchantments/owner.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minions.h"
#include "game-engine/enchantments/managed-enchantment.h"
#include "game-engine/hook/minion-aura.h"

namespace GameEngine
{
	template<>
	inline Enchantments<Minion>::~Enchantments()
	{
#ifdef DEBUG
		if (!this->enchantments.Empty()) {
			throw std::runtime_error("minion enchantments should be cleared first");
		}
#endif
	}

	template <typename Target> inline Enchantments<Target>::~Enchantments() {}

	template <typename Target>
	inline bool Enchantments<Target>::operator==(Enchantments const & rhs) const
	{
		auto comparator = [this, &rhs] (ItemType const& lhs_item, ItemType const& rhs_item) {
			return lhs_item.EqualsTo(rhs_item, this->holder, rhs.holder);
		};
		return this->enchantments.EqualsTo(rhs.enchantments, comparator);
	}

	template <typename Target>
	inline bool Enchantments<Target>::operator!=(Enchantments const & rhs) const
	{
		return !(*this == rhs);
	}

	template<typename Target>
	inline void Enchantments<Target>::DestroyBoard()
	{
		this->enchantments.RemoveIf([this](auto) {
			return true;
		});
	}

	template <typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, MinionAura & aura)
	{
		EnchantmentsOwner<Target> * owner;
		aura.GetEnchantmentsOwner(owner);
		return this->Add(std::move(enchantment), owner);
	}

	template <typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentsOwner<Target> * owner)
	{
		using OwnerToken = typename EnchantmentTypes<Target>::OwnerToken;

		auto holder_token = this->holder.Add(std::move(enchantment));

		auto managed_item = this->enchantments.PushBack(ItemType(holder_token, owner));
		
		if (owner)
		{
			OwnerToken owner_token = owner->EnchantmentAdded(ManagedEnchantment<Target>(*this, managed_item));
			managed_item->owner_token = new OwnerToken(owner_token);
		}

		this->holder.Get(holder_token)->Apply(this->target);
	}

	template <typename Target>
	inline void Enchantments<Target>::Remove(ManagedEnchantment<Target> & item) 
	{
		// Note: the following line should be a copy, rather than a reference
		// Since the parameter 'item' is a referece from owner's enchantments list
		// which will be erased when 'this->BeforeRemove()'
		auto managed_item = item.Get(); // Note: need to use copy (not reference)

		this->BeforeRemove(managed_item);
		managed_item.Remove();
	}

	template <typename Target>
	inline void Enchantments<Target>::Clear()
	{
		this->enchantments.RemoveIf([this](auto item) {
			this->BeforeRemove(item);
			return true;
		});
	}

	template<typename Target>
	inline bool Enchantments<Target>::Empty() const
	{
		return this->enchantments.Empty();
	}

	template <typename Target>
	inline void Enchantments<Target>::TurnEnd()
	{
		this->enchantments.RemoveIf([this](auto item) {
			bool expired;
			this->holder.Get(item->holder_token)->TurnEnd(this->target, expired);

			if (expired) {
				this->BeforeRemove(item);
				return true; // enchant expired --> remove it
			}
			else {
				return false;
			}
		});
	}

	template <typename Target>
	inline void Enchantments<Target>::BeforeRemove(ManagedItem item)
	{
		this->holder.Get(item->holder_token)->Remove(this->target);
		if (item->owner) item->owner->EnchantmentRemoved(*item->owner_token);

		this->holder.Remove(item->holder_token);
	}
} // namespace GameEngine

template <typename Target>
inline std::size_t std::hash<GameEngine::Enchantments<Target>>::operator()(const argument_type &s) const
{
	result_type result = 0;

	s.enchantments.ForEach([&s, &result](auto const& enchantment) {
		GameEngine::Enchantment<Target> * enchant = s.holder.Get(enchantment.holder_token).get();
		GameEngine::hash_combine(result, *enchant);
	});

	return result;
}