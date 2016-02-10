#pragma once

#include <list>
#include <map>

namespace GameEngine {
namespace BoardObjects {

class Minion;
class Enchantment;
class EnchantmentOwner;

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

	void Add(Enchantment * enchantment, Minion * target, EnchantmentOwner * owner);
	void Remove(Enchantment * enchantment, Minion * target);
	void Clear(Minion * target);

public: // hooks
	void TurnEnd(Minion * target);

private:
	typedef std::list<std::pair<Enchantment *, EnchantmentOwner*>> container_type;

	container_type::iterator Remove(container_type::iterator enchantment, Minion * target);

	container_type enchantments;
};

class EnchantmentOwner
{
public:
	void RemoveOwnedEnchantments();

	// hooks
	void EnchantmentAdded(Enchantment * enchantment, Minion * target) { this->enchantments[enchantment] = target; }
	void EnchantmentRemoved(Enchantment * enchantment, Minion *) { this->enchantments.erase(enchantment); }

private:
	std::map<Enchantment *, Minion *> enchantments;
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