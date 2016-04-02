#include "game-engine\enchantments\enchantments.h"
#include "game-engine\enchantments\enchantment.h"
#include "game-engine\board-objects\minion-data.h"
#include "game-engine\board-objects\minion.h"
#include "game-engine\board-objects\minions.h"

template <typename Target>
inline bool GameEngine::BoardObjects::Enchantments<Target>::operator==(Enchantments const & rhs) const
{
	if (this->enchantments.size() != rhs.enchantments.size()) return false;

	auto it_lhs = this->enchantments.begin();
	auto it_rhs = rhs.enchantments.begin();
	while (true)
	{
		if (it_lhs == this->enchantments.end()) break;
		if (it_rhs == rhs.enchantments.end()) break;

		if (*(it_lhs->first) != *(it_rhs->first)) return false;

		it_lhs++;
		it_rhs++;
	}
	// the size of both lists are equal, so here it_lhs and it_rhs should both reaches end

	return true;
}

template <typename Target>
inline bool GameEngine::BoardObjects::Enchantments<Target>::operator!=(Enchantments const & rhs) const
{
	return !(*this == rhs);
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::Add(
	std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentOwner * owner, Target & target)
{
	auto ref_ptr = enchantment.get();

	this->enchantments.push_back(std::make_pair(std::move(enchantment), owner));
	
	if (owner) owner->EnchantmentAdded(ref_ptr);
	ref_ptr->AfterAdded(target);
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::Remove(
	Enchantment<Target> * enchantment, Target & target)
{
	// A O(N) algorithm, since the enchantment should not be large in a normal play
	for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it) {
		if (it->first.get() == enchantment) {
			// found, remove it
			this->Remove(it, target);
			return;
		}
	}
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::Clear(Target & target)
{
	for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
	{
		it = this->Remove(it, target);
	}
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::TurnEnd(Target & target)
{
	for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
	{
		if (it->first->TurnEnd(target) == false) {
			// enchant vanished
			it = this->Remove(it, target);
		}
		else {
			it++;
		}
	}
}

template <typename Target>
inline typename GameEngine::BoardObjects::Enchantments<Target>::container_type::iterator GameEngine::BoardObjects::Enchantments<Target>::Remove(
	typename container_type::iterator it, Target & target)
{
	it->first->BeforeRemoved(target);
	if (it->second) it->second->EnchantmentRemoved(it->first.get());
	
	return this->enchantments.erase(it);
}

template <typename Target>
inline std::size_t std::hash<GameEngine::BoardObjects::Enchantments<Target>>::operator()(const argument_type &s) const
{
	result_type result = 0;

	for (auto const& enchantment : s.enchantments)
	{
		GameEngine::hash_combine(result, *enchantment.first);
	}

	return result;
}
