#pragma once

#include <list>
#include <algorithm>

namespace GameEngine {

class MinionData;
class Minion;

template <typename Target>
class Enchantment
{
	friend std::hash<Enchantment>;

public:
	Enchantment() : applied(false) {}
	virtual ~Enchantment() {}

	bool operator==(Enchantment<Target> const& rhs) const { return this->EqualsTo(rhs); }
	bool operator!=(Enchantment<Target> const& rhs) const { return !(*this == rhs); }

public: // hooks
	virtual void AfterAdded(Target & target)
	{
		(void)target;
#ifdef DEBUG
		if (this->applied) throw std::runtime_error("enchantment already applied");
		this->applied = true;
#endif
	}
	virtual void BeforeRemoved(Target & target)
	{
		(void)target;
#ifdef DEBUG
		if (!this->applied) throw std::runtime_error("enchantment not applied");
		// do not set this->applied to false, since a enchantment is not designed to be applied twice
#endif
	}

	// return false if enchant vanished
	virtual void TurnEnd(Target & target, bool & expired) { (void)target; expired = false; }

protected:
	virtual bool EqualsTo(Enchantment<Target> const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
	virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)

private:
#ifdef DEBUG
	bool applied;
#endif
};

} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::Enchantment<Target> > {
		typedef GameEngine::Enchantment<Target> argument_type;
		typedef std::size_t result_type;

		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}