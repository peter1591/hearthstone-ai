#include "game-engine/enchantments/enchantments.h"
#include "game-engine/enchantments/enchantment.h"
#include "game-engine/enchantments/owner.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minions.h"
#include "game-engine/enchantments/owner-item.h"
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

	template<typename Target>
	inline typename Enchantments<Target>::ManagedItem
	Enchantments<Target>::Add(std::unique_ptr<EnchantmentType>&& enchantment)
	{
		using OwnerToken = typename EnchantmentTypes<Target>::OwnerToken;

		auto holder_token = this->holder.Add(std::move(enchantment));

		auto managed_item = this->enchantments.PushBack(ItemType(holder_token));

		this->holder.Get(holder_token)->Apply(this->target);

		return managed_item;
	}

	template<typename Target>
	inline void Enchantments<Target>::Remove(ManagedItem managed_item)
	{
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
			this->holder.Get(item->GetHolderToken())->TurnEnd(this->target, expired);

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
		this->holder.Get(item->GetHolderToken())->Remove(this->target);
		if (item->GetOwner()) item->GetOwner()->RemoveEnchantment(item->GetOwnerToken());

		this->holder.Remove(item->GetHolderToken());
	}
} // namespace GameEngine

template <typename Target>
inline std::size_t std::hash<GameEngine::Enchantments<Target>>::operator()(const argument_type &s) const
{
	result_type result = 0;

	s.enchantments.ForEach([&s, &result](auto const& enchantment) {
		GameEngine::Enchantment<Target> * enchant = s.holder.Get(enchantment.GetHolderToken()).get();
		GameEngine::hash_combine(result, *enchant);
	});

	return result;
}