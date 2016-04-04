#include "game-engine/enchantments/enchantments.h"
#include "game-engine/enchantments/enchantment.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minions.h"
#include "game-engine/enchantments/managed-enchantment.h"
#include "game-engine/hook/minion-aura.h"

namespace GameEngine
{
	template<typename Target>
	inline Enchantments<Target>::~Enchantments()
	{
#ifdef DEBUG
		if (!this->enchantments.Empty()) {
			throw std::runtime_error("enchantments should be cleared first");
		}
#endif
	}

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

	template<typename Target>
	inline void Enchantments<Target>::Destroy()
	{
		this->enchantments.RemoveIf([this](auto item) {
			return true;
		});
	}

	template<typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, MinionAura & aura)
	{
		return this->Add(std::move(enchantment), &aura.minion_enchantments);
	}

	template <typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentOwner<Target> * owner)
	{
		auto ref_ptr = enchantment.get();

		auto managed_item = this->enchantments.PushBack(ItemType(std::move(enchantment), owner));
		
		if (owner) owner->EnchantmentAdded(ManagedEnchantment<Target>(*this, managed_item));

		ref_ptr->AfterAdded(this->target);
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
			item->enchantment->TurnEnd(this->target, expired);

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
	inline void Enchantments<Target>::BeforeRemove(typename ManagedContainer::ManagedItem item)
	{
		item->enchantment->BeforeRemoved(this->target);
		if (item->owner) item->owner->EnchantmentRemoved(item);
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