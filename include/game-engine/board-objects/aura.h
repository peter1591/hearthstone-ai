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
			virtual void BeforeRemoved(Board & board, MinionsIteratorWithIndex & owner) {}

			virtual void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, MinionsIteratorWithIndex & added_minion) {}
			virtual void HookAfterOwnerEnraged(Board & board, BoardObjects::MinionsIteratorWithIndex &enraged_aura_owner) {}
			virtual void HookAfterOwnerUnEnraged(Board & board, BoardObjects::MinionsIteratorWithIndex &unenraged_aura_owner) {}

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

			void Clear(Board & board, MinionsIteratorWithIndex & owner)
			{
				for (auto & aura : this->auras) aura->BeforeRemoved(board, owner);
				this->auras.clear();
			}

		public: // hooks
			void HookAfterMinionAdded(Board & board, BoardObjects::MinionsIteratorWithIndex &aura_owner, MinionsIteratorWithIndex & added_minion)
			{
				for (auto & aura : this->auras) aura->HookAfterMinionAdded(board, aura_owner, added_minion);
			}
			void HookAfterOwnerEnraged(Board & board, BoardObjects::MinionsIteratorWithIndex &enraged_aura_owner)
			{
				for (auto & aura : this->auras) aura->HookAfterOwnerEnraged(board, enraged_aura_owner);
			}
			void HookAfterOwnerUnEnraged(Board & board, BoardObjects::MinionsIteratorWithIndex &unenraged_aura_owner)
			{
				for (auto & aura : this->auras) aura->HookAfterOwnerUnEnraged(board, unenraged_aura_owner);
			}

		private:
			std::list<Aura*> auras;
		};

	} // namespace BoardObjects
} // namespace GameEngine