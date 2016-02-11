#include "game-engine\board-objects\enchantments.h"
#include "game-engine\board-objects\enchantment.h"
#include "game-engine\board-objects\minion.h"
#include "game-engine\board-objects\minion-manipulator.h"
#include "game-engine\board-objects\minions.h"

inline GameEngine::BoardObjects::Enchantments::~Enchantments()
{
	for (auto const& item : this->enchantments)
	{
		delete item.first;
	}
}

inline void GameEngine::BoardObjects::Enchantments::CheckCanBeSafelyCloned() const
{
	// Since enchantments are placed on heap, which is not safe by shallow copy
	if (!this->enchantments.empty()) {
		throw std::runtime_error("You should not clone a board with enchantments.");
	}
}

inline bool GameEngine::BoardObjects::Enchantments::operator==(Enchantments const & rhs) const
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

inline bool GameEngine::BoardObjects::Enchantments::operator!=(Enchantments const & rhs) const
{
	return !(*this == rhs);
}

inline void GameEngine::BoardObjects::Enchantments::Add(Enchantment * enchantment, EnchantmentOwner * owner, GameEngine::BoardObjects::MinionManipulator const& target)
{
	this->enchantments.push_back(std::make_pair(enchantment, owner));
	if (owner) owner->EnchantmentAdded(enchantment, target);
	enchantment->AfterAdded(target);
}

inline void GameEngine::BoardObjects::Enchantments::Remove(Enchantment * enchantment, GameEngine::BoardObjects::MinionManipulator const& target)
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

inline void GameEngine::BoardObjects::Enchantments::Clear(GameEngine::BoardObjects::MinionManipulator const& target)
{
	for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
	{
		it = this->Remove(it, target);
	}
}

inline void GameEngine::BoardObjects::Enchantments::TurnEnd(GameEngine::BoardObjects::MinionManipulator const& target)
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

inline GameEngine::BoardObjects::Enchantments::container_type::iterator GameEngine::BoardObjects::Enchantments::Remove(
	container_type::iterator it, GameEngine::BoardObjects::MinionManipulator const& target)
{
	it->first->BeforeRemoved(target);
	if (it->second) it->second->EnchantmentRemoved(it->first, target);
	
	delete it->first;

	return this->enchantments.erase(it);
}

inline std::size_t std::hash<GameEngine::BoardObjects::Enchantments>::operator()(const argument_type &s) const
{
	result_type result = 0;

	for (auto const& enchantment : s.enchantments)
	{
		GameEngine::hash_combine(result, *enchantment.first);
	}

	return result;
}

inline void GameEngine::BoardObjects::EnchantmentOwner::RemoveOwnedEnchantments(GameEngine::BoardObjects::MinionManipulator const& owner)
{
	while (!this->enchantments.empty()) {
		auto it = this->enchantments.begin();
		owner.minions->GetManipulator(*owner.board, it->second).RemoveEnchantment(it->first);
	}
}

inline void GameEngine::BoardObjects::EnchantmentOwner::EnchantmentAdded(Enchantment * enchantment, MinionManipulator const& target)
{
	this->enchantments[enchantment] = target.minion;
}

inline void GameEngine::BoardObjects::EnchantmentOwner::EnchantmentRemoved(Enchantment * enchantment, MinionManipulator const& target)
{
	this->enchantments.erase(enchantment);
}
