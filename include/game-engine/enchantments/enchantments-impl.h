#include "game-engine/enchantments/enchantments.h"
#include "game-engine/enchantments/enchantment.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minions.h"
#include "game-engine/enchantments/managed-enchantment.h"

namespace GameEngine
{
	template <typename Target>
	inline bool Enchantments<Target>::operator==(Enchantments const & rhs) const
	{
		return this->enchantments == rhs.enchantments;
	}

	template <typename Target>
	inline bool Enchantments<Target>::operator!=(Enchantments const & rhs) const
	{
		return !(*this == rhs);
	}

	template <typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentOwner<Target> * owner)
	{
		auto ref_ptr = enchantment.get();

		auto managed_item = this->enchantments.PushBack(ItemType(std::move(enchantment), owner));
		
		if (owner) owner->EnchantmentAdded(ManagedEnchantment<Target>(*this, managed_item));

		ref_ptr->AfterAdded(target);
	}

	template <typename Target>
	inline void Enchantments<Target>::Remove(ManagedEnchantment<Target> & item)
	{
		auto & managed_item = item.Get();

		this->BeforeRemove(*managed_item);
		managed_item.Remove();
	}

	template <typename Target>
	inline void Enchantments<Target>::Clear()
	{
		this->enchantments.RemoveIf([this](auto & item) {
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
		this->enchantments.RemoveIf([this](auto & item) {
			if (item.enchantment->TurnEnd(this->target)) {
				this->BeforeRemove(item);
				return true; // enchant vanished if return value is true
			}
			else {
				return false;
			}
		});
	}

	template <typename Target>
	inline void Enchantments<Target>::BeforeRemove(typename Enchantments<Target>::ItemType & item)
	{
		item.enchantment->BeforeRemoved(this->target);
		if (item.owner) item.owner->EnchantmentRemoved(item);
	}
} // namespace GameEngine

template <typename Target>
inline std::size_t std::hash<GameEngine::Enchantments<Target>>::operator()(const argument_type &s) const
{
	result_type result = 0;

	s.enchantments.ForEach([&result](auto const& enchantment) {
		GameEngine::hash_combine(result, enchantment);
	});

	return result;
}