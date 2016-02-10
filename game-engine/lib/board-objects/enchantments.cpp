#include "game-engine\board-objects\enchantments.h"
#include "game-engine\board-objects\enchantment.h"
#include "game-engine\board-objects\minion.h"

GameEngine::BoardObjects::Enchantments::~Enchantments()
{
	for (auto const& item : this->enchantments)
	{
		delete item.first;
	}
}

void GameEngine::BoardObjects::Enchantments::CheckCanBeSafelyCloned() const
{
	// Since enchantments are placed on heap, which is not safe by shallow copy
	if (!this->enchantments.empty()) {
		throw std::runtime_error("You should not clone a board with enchantments.");
	}
}

bool GameEngine::BoardObjects::Enchantments::operator==(Enchantments const & rhs) const
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

bool GameEngine::BoardObjects::Enchantments::operator!=(Enchantments const & rhs) const
{
	return !(*this == rhs);
}

void GameEngine::BoardObjects::Enchantments::Add(Enchantment * enchantment, Minion * target, EnchantmentOwner * owner)
{
	this->enchantments.push_back(std::make_pair(enchantment, owner));
	if (owner) owner->EnchantmentAdded(enchantment, target);
	enchantment->AfterAdded(target);
}

void GameEngine::BoardObjects::Enchantments::Remove(Enchantment * enchantment, Minion * target)
{
	// A O(N) algorithm, since the enchantment should not be large in a normal play
	for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it)
	{
		if (it->first == enchantment)
		{
			// found -> remove it
			this->Remove(it, target);
			return;
		}
	}

	throw std::runtime_error("enchantment not found");
}

void GameEngine::BoardObjects::Enchantments::Clear(Minion * target)
{
	for (container_type::iterator it = this->enchantments.begin(); it != this->enchantments.end();)
	{
		it = this->Remove(it, target);
	}
}

void GameEngine::BoardObjects::Enchantments::TurnEnd(Minion * target)
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

GameEngine::BoardObjects::Enchantments::container_type::iterator GameEngine::BoardObjects::Enchantments::Remove(container_type::iterator it, Minion * target)
{
	it->first->BeforeRemoved(target);
	if (it->second) it->second->EnchantmentRemoved(it->first, target);
	
	delete it->first;

	return this->enchantments.erase(it);
}

std::size_t std::hash<GameEngine::BoardObjects::Enchantments>::operator()(const argument_type &s) const 
{
	result_type result = 0;

	for (auto const& enchantment : s.enchantments)
	{
		GameEngine::hash_combine(result, *enchantment.first);
	}

	return result;
}

void GameEngine::BoardObjects::EnchantmentOwner::RemoveOwnedEnchantments()
{
	while (!this->enchantments.empty())
	{
		auto it = this->enchantments.begin();
		it->second->RemoveEnchantment(it->first); // this will actually remove the iterator 'it'
	}
}