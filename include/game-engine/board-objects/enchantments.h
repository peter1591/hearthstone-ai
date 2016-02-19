#pragma once

#include <list>
#include <map>
#include <memory>

namespace GameEngine {
namespace BoardObjects {

class Minion;
template <typename Target> class Enchantment;
class EnchantmentOwner;
class MinionManipulator;

// Maintains the lifetime of Enchantment
template <typename Target>
class Enchantments
{
	friend std::hash<Enchantments>;

public:
	Enchantments() {}

	Enchantments(Enchantments<Target> const& rhs) = delete;
	Enchantments<Target> operator=(Enchantments<Target> const& rhs) = delete;
	
	Enchantments(Enchantments<Target> && rhs) { *this = std::move(rhs); }
	Enchantments<Target> & operator=(Enchantments<Target> && rhs) {
		this->enchantments = std::move(rhs.enchantments);
		return *this;
	}

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentOwner * owner, Target & minion);
	void Remove(Enchantment<Target> * enchantment, Target & target);
	void Clear(Target & target);
	bool Empty() const { return this->enchantments.empty(); }

public: // hooks
	void TurnEnd(Target & target);

private:
	typedef typename std::list<std::pair<std::unique_ptr<Enchantment<Target>>, EnchantmentOwner*>> container_type;

	typename container_type::iterator Remove(typename container_type::iterator it, Target & target);

	container_type enchantments;
};

class EnchantmentOwner
{
public:
	bool IsEmpty() const;

	void RemoveOwnedEnchantments(GameEngine::BoardObjects::MinionManipulator & owner);

	// hooks
	void EnchantmentAdded(Enchantment<MinionManipulator> * enchantment);
	void EnchantmentRemoved(Enchantment<MinionManipulator> * enchantment);

private:
	std::list<Enchantment<MinionManipulator> *> minion_enchantments;
};

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::BoardObjects::Enchantments<Target> > {
		typedef GameEngine::BoardObjects::Enchantments<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}