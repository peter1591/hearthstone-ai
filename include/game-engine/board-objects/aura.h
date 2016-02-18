#pragma once

#include <list>

namespace GameEngine {
	class Board;

	namespace BoardObjects {

		class Minion;
		class MinionManipulator;

		class Aura
		{
			friend std::hash<Aura>;

		public:
			Aura() {}
			virtual ~Aura() {}

			bool operator==(Aura const& rhs) const { return this->EqualsTo(rhs); }
			bool operator!=(Aura const& rhs) const { return !(*this == rhs); }

		public: // hooks
			virtual void AfterAdded(MinionManipulator & owner) {}
			virtual void BeforeRemoved(MinionManipulator & owner) {}

			virtual void HookAfterMinionAdded(MinionManipulator & aura_owner, MinionManipulator & added_minion) {}
			virtual void HookAfterOwnerEnraged(MinionManipulator &enraged_aura_owner) {}
			virtual void HookAfterOwnerUnEnraged(MinionManipulator &unenraged_aura_owner) {}

		protected:
			virtual bool EqualsTo(Aura const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
			virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
		};

		class Auras
		{
			friend std::hash<Auras>;

		public:
			Auras() {}
			~Auras()
			{
				for (auto const& aura : this->auras)
				{
					delete aura;
				}
			}

			Auras(Auras const& rhs) = delete;

			// default copy constructor/assignment are used for shallow copy
			// deep copy (i.e., Clone) are prevented by the following function.
			void CheckCanBeSafelyCloned() const
			{
				// Since auras are placed on heap, which is not safe by shallow copy
				if (!this->auras.empty()) {
					throw std::runtime_error("You should not clone a board with auras.");
				}
			}

			bool operator==(Auras const& rhs) const
			{
				if (this->auras.size() != rhs.auras.size()) return false;

				auto it_lhs = this->auras.begin();
				auto it_rhs = rhs.auras.begin();

				while (true)
				{
					if (it_lhs == this->auras.end()) break;
					if (it_rhs == rhs.auras.end()) break;

					if (*it_lhs != *it_rhs) return false;
				}
				// both iterators should reach end here, since the size is equal

				return true;
			}

			bool operator!=(Auras const& rhs) const { return !(*this == rhs); }

		public:
			void Add(MinionManipulator & owner, Aura* aura)
			{
				this->auras.push_back(aura);
				aura->AfterAdded(owner);
			}

			void Clear(MinionManipulator & owner)
			{
				for (auto & aura : this->auras) aura->BeforeRemoved(owner);
				this->auras.clear();
			}

		public: // hooks
			void HookAfterMinionAdded(MinionManipulator & aura_owner, MinionManipulator & added_minion)
			{
				for (auto & aura : this->auras) aura->HookAfterMinionAdded(aura_owner, added_minion);
			}
			void HookAfterOwnerEnraged(MinionManipulator &enraged_aura_owner)
			{
				for (auto & aura : this->auras) aura->HookAfterOwnerEnraged(enraged_aura_owner);
			}
			void HookAfterOwnerUnEnraged(MinionManipulator &unenraged_aura_owner)
			{
				for (auto & aura : this->auras) aura->HookAfterOwnerUnEnraged(unenraged_aura_owner);
			}

		private:
			std::list<Aura*> auras;
		};

	} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Aura> {
		typedef GameEngine::BoardObjects::Aura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
	template <> struct hash<GameEngine::BoardObjects::Auras> {
		typedef GameEngine::BoardObjects::Auras argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto const& aura : s.auras) {
				GameEngine::hash_combine(result, aura);
			}

			return result;
		}
	};
}