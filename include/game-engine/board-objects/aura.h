#pragma once

#include <list>

namespace GameEngine {
	class Board;

	namespace BoardObjects {

		class Minion;
		class MinionsIteratorWithIndex;

		class Aura
		{
		public:
			Aura() {}
			virtual ~Aura() {}

			bool operator==(Aura const& rhs) const { return this->EqualsTo(rhs); }
			bool operator!=(Aura const& rhs) const { return !(*this == rhs); }

		public: // hooks
			virtual void AfterAdded(Board & board, MinionsIteratorWithIndex & owner) {}
			virtual void BeforeRemoved() {}

			virtual void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, MinionsIteratorWithIndex & added_minion) {}

		protected:
			virtual bool EqualsTo(Aura const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
			virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
		};

		class Auras
		{
		public:
			Auras() {}
			~Auras()
			{
				for (auto const& aura : this->auras)
				{
					delete aura;
				}
			}

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
			void Add(Board & board, MinionsIteratorWithIndex & owner, Aura* aura)
			{
				this->auras.push_back(aura);
				aura->AfterAdded(board, owner);
			}

			void Clear()
			{
				for (auto & aura : this->auras) aura->BeforeRemoved();
				this->auras.clear();
			}

		public: // hooks
			void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, MinionsIteratorWithIndex & added_minion)
			{
				for (auto & aura : this->auras) aura->HookAfterMinionAdded(board, aura_owner, added_minion);
			}

		private:
			std::list<Aura*> auras;
		};

	} // namespace BoardObjects
} // namespace GameEngine