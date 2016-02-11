#pragma once

#include <list>
#include <map>

namespace GameEngine {
namespace BoardObjects {

class Minion;
class Enchantment;
class EnchantmentOwner;
class MinionManipulator;

// Maintains the lifetime of Enchantment
class Enchantments
{
	friend std::hash<Enchantments>;

public:
	Enchantments() {}
	~Enchantments();

	// default copy constructor/assignment are used for shallow copy
	// deep copy (i.e., Clone) are prevented by the following function.
	void CheckCanBeSafelyCloned() const;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(Enchantment * enchantment, EnchantmentOwner * owner, GameEngine::BoardObjects::MinionManipulator const& minion);
	void Remove(Enchantment * enchantment, GameEngine::BoardObjects::MinionManipulator const& target);
	void Clear(GameEngine::BoardObjects::MinionManipulator const& target);

public: // hooks
	void TurnEnd(GameEngine::BoardObjects::MinionManipulator const& target);

private:
	typedef std::list<std::pair<Enchantment *, EnchantmentOwner*>> container_type;

	container_type::iterator Remove(container_type::iterator enchantment, GameEngine::BoardObjects::MinionManipulator const& target);

	container_type enchantments;
};

class EnchantmentOwner
{
public:
	void RemoveOwnedEnchantments(GameEngine::BoardObjects::MinionManipulator const& owner);

	// hooks
	void EnchantmentAdded(Enchantment * enchantment, MinionManipulator const& target);
	void EnchantmentRemoved(Enchantment * enchantment, MinionManipulator const& target);

private:
	std::map<Enchantment *, Minion const*> enchantments;
};

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Enchantments> {
		typedef GameEngine::BoardObjects::Enchantments argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}