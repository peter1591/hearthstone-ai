#pragma once

#include <list>
#include <map>

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
	~Enchantments();

	// default copy constructor/assignment are used for shallow copy
	// deep copy (i.e., Clone) are prevented by the following function.
	void CheckCanBeSafelyCloned() const;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(Enchantment<Target> * enchantment, EnchantmentOwner * owner, Target & minion);
	void Remove(Enchantment<Target> * enchantment, Target & target);
	void Clear(Target & target);

public: // hooks
	void TurnEnd(Target & target);

private:
	typedef typename std::list<std::pair<Enchantment<Target> *, EnchantmentOwner*>> container_type;

	typename container_type::iterator Remove(typename container_type::iterator it, Target & target);

	container_type enchantments;
};

class EnchantmentOwner
{
public:
	bool IsEmpty() const;

	void RemoveOwnedEnchantments(GameEngine::BoardObjects::MinionManipulator & owner);

	// hooks
	void EnchantmentAdded(Enchantment<MinionManipulator> * enchantment, MinionManipulator & target);
	void EnchantmentRemoved(Enchantment<MinionManipulator> * enchantment, MinionManipulator & target);

private:
	std::map<Enchantment<MinionManipulator> *, Minion const*> minion_enchantments;
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