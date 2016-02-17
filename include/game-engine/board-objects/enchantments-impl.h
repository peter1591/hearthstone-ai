#include "game-engine\board-objects\enchantments.h"
#include "game-engine\board-objects\enchantment.h"
#include "game-engine\board-objects\minion.h"
#include "game-engine\board-objects\minion-manipulator.h"
#include "game-engine\board-objects\minions.h"

template <typename Target>
inline GameEngine::BoardObjects::Enchantments<Target>::~Enchantments()
{
	for (auto const& item : this->enchantments)
	{
		delete item.first;
	}
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::CheckCanBeSafelyCloned() const
{
	// Since enchantments are placed on heap, which is not safe by shallow copy
	if (!this->enchantments.empty()) {
		throw std::runtime_error("You should not clone a board with enchantments.");
	}
}

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
	Enchantment<Target> * enchantment, EnchantmentOwner * owner, Target const& target)
{
	this->enchantments.push_back(std::make_pair(enchantment, owner));
	if (owner) owner->EnchantmentAdded(enchantment, target);
	enchantment->AfterAdded(target);
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::Remove(
	Enchantment<Target> * enchantment, Target const& target)
{
	// A O(N) algorithm, since the enchantment should not be large in a normal play
	for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it)
	{
		if (it->first == enchantment)
		{
			// found, remove it
			this->Remove(it, target);
			return;
		}
	}

	throw std::runtime_error("enchantment not found");
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::Clear(Target const& target)
{
	for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
	{
		it = this->Remove(it, target);
	}
}

template <typename Target>
inline void GameEngine::BoardObjects::Enchantments<Target>::TurnEnd(Target const& target)
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
	typename container_type::iterator it, Target const& target)
{
	it->first->BeforeRemoved(target);
	if (it->second) it->second->EnchantmentRemoved(it->first, target);
	
	delete it->first;

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

inline bool GameEngine::BoardObjects::EnchantmentOwner::IsEmpty() const
{
	return this->minion_enchantments.empty();
}

inline void GameEngine::BoardObjects::EnchantmentOwner::RemoveOwnedEnchantments(GameEngine::BoardObjects::MinionManipulator const& owner)
{
	while (!this->minion_enchantments.empty()) {
		auto it = this->minion_enchantments.begin();
		owner.GetMinions().GetManipulator(it->second).RemoveEnchantment(it->first);
	}
}

inline void GameEngine::BoardObjects::EnchantmentOwner::EnchantmentAdded(
	Enchantment<GameEngine::BoardObjects::MinionManipulator> * enchantment, MinionManipulator const& target)
{
	this->minion_enchantments[enchantment] = &target.GetMinion();
}

inline void GameEngine::BoardObjects::EnchantmentOwner::EnchantmentRemoved(
	Enchantment<GameEngine::BoardObjects::MinionManipulator> * enchantment, MinionManipulator const& target)
{
	this->minion_enchantments.erase(enchantment);
}
