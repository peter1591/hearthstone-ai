#pragma once

#include <list>
#include <map>
#include <memory>

namespace GameEngine {

class MinionData;
template <typename Target> class Enchantment;
class EnchantmentOwner;
class Minion;

// Maintains the lifetime of Enchantment
template <typename Target>
class Enchantments
{
	friend std::hash<Enchantments>;

public:
	Enchantments() {}

	Enchantments(Enchantments<Target> const& rhs) = delete;
	Enchantments<Target> operator=(Enchantments<Target> const& rhs) = delete;
	Enchantments(Enchantments<Target> && rhs);
	Enchantments<Target> & operator=(Enchantments<Target> && rhs) = delete;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentOwner * owner, Target & minion);
	void Remove(Enchantment<Target> * enchantment, Target & target);
	void Clear(Target & target);
	bool Empty() const;

public: // hooks
	void TurnEnd(Target & target);

private:
	typedef typename std::list<std::pair<std::unique_ptr<Enchantment<Target>>, EnchantmentOwner*>> container_type;

	typename container_type::iterator Remove(typename container_type::iterator it, Target & target);

	container_type enchantments;
};

} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::Enchantments<Target> > {
		typedef GameEngine::Enchantments<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}