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
		virtual void TurnStart(bool owner_turn) { (void)owner_turn; }
		virtual void TurnEnd(bool owner_turn) { (void)owner_turn; }

		virtual void HookAfterMinionAdded(Minion & added_minion) { (void)added_minion; }
		virtual void HookAfterOwnerEnraged() {}
		virtual void HookAfterOwnerUnEnraged() {}
		virtual void HookAfterMinionDamaged(Minion & minion, int damage) { (void)minion; (void)damage; }
		virtual void HookBeforeMinionTransform(Minion & minion, int new_card_id) { (void)minion; (void)new_card_id; }
		virtual void HookAfterMinionTransformed(Minion & minion) { (void)minion; }

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