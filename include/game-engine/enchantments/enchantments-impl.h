#include "game-engine\enchantments\enchantments.h"
#include "game-engine\enchantments\enchantment.h"
#include "game-engine/enchantments/managed-enchantment.h"
#include "game-engine\board-objects\minion-data.h"
#include "game-engine\board-objects\minion.h"
#include "game-engine\board-objects\minions.h"

namespace GameEngine
{

	template<typename Target>
	inline Enchantments<Target>::Enchantments(Target & target, Enchantments<Target>&& rhs)
		: target(target), enchantments(std::move(rhs.enchantments))
	{
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

	template <typename Target>
	inline void Enchantments<Target>::Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentOwner * owner)
	{
		auto ref_ptr = enchantment.get();

		this->enchantments.push_back(ItemType(std::move(enchantment), owner));

		if (owner) owner->EnchantmentAdded(ref_ptr);
		ref_ptr->AfterAdded(target);
	}

	template <typename Target>
	inline void Enchantments<Target>::Remove(EnchantmentType * enchantment)
	{
		// A O(N) algorithm, since the enchantment should not be large in a normal play
		for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it) {
			if (it->enchantment.get() == enchantment) {
				// found, remove it
				this->Remove(it);
				return;
			}
		}
	}

	template <typename Target>
	inline void Enchantments<Target>::Clear()
	{
		for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
		{
			it = this->Remove(it);
		}
	}

	template<typename Target>
	inline bool Enchantments<Target>::Empty() const
	{
		return this->enchantments.empty();
	}

	template <typename Target>
	inline void Enchantments<Target>::TurnEnd()
	{
		for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
		{
			if (it->enchantment->TurnEnd(this->target) == false) {
				// enchant vanished
				it = this->Remove(it);
			}
			else {
				it++;
			}
		}
	}

	template <typename Target>
	inline typename Enchantments<Target>::container_type::iterator Enchantments<Target>::Remove(typename container_type::iterator it)
	{
		it->enchantment->BeforeRemoved(this->target);
		if (it->owner) it->owner->EnchantmentRemoved(it->enchantment.get());

		return this->enchantments.erase(it);
	}

} // namespace GameEngine

template <typename Target>
inline std::size_t std::hash<GameEngine::Enchantments<Target>>::operator()(const argument_type &s) const
{
	result_type result = 0;

	for (auto const& enchantment : s.enchantments)
	{
		GameEngine::hash_combine(result, *enchantment.enchantment);
	}

	return result;
}