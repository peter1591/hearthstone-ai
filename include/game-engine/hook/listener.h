#pragma once

#include <functional>

namespace GameEngine {
	class Minion;
	class HookListener
	{
		friend std::hash<HookListener>;

	public:
		HookListener() {}
		virtual ~HookListener() {}

		bool operator==(HookListener const& rhs) const { return this->EqualsTo(rhs); }
		bool operator!=(HookListener const& rhs) const { return !(*this == rhs); }

	public: // hooks
		virtual void AfterAdded(Minion & owner) { (void)owner; }
		virtual void BeforeRemoved(Minion & owner) { (void)owner; }

		virtual void HookAfterMinionAdded(Minion & aura_owner, Minion & added_minion) { (void)aura_owner; (void)added_minion; }
		virtual void HookAfterOwnerEnraged(Minion &enraged_aura_owner) { (void)enraged_aura_owner; }
		virtual void HookAfterOwnerUnEnraged(Minion &unenraged_aura_owner) { (void)unenraged_aura_owner; }
		virtual void HookAfterMinionDamaged(Minion & minion, int damage) { (void)minion; (void)damage; }

	protected:
		virtual bool EqualsTo(HookListener const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
		virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
	};
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::HookListener> {
		typedef GameEngine::HookListener argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}