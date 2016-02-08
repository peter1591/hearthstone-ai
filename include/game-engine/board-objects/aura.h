#pragma once

#include <list>

namespace GameEngine {
	class Board;

	namespace BoardObjects {

		class Minion;

		class Aura
		{
		public:
			Aura() {}
			virtual ~Aura() {}

			bool operator==(Aura const& rhs) const { return this->EqualsTo(rhs); }
			bool operator!=(Aura const& rhs) const { return !(*this == rhs); }

		public: // hooks
			virtual void BeforeRemoved() {}

			virtual void AfterMinionSummoned(Board &, Minion &) {}

		protected:
			virtual bool EqualsTo(Aura const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
			virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
		};

		class Auras
		{
		public:

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

			void Add(Aura* aura)
			{
				this->auras.push_back(aura);
			}

			void Clear()
			{
				for (auto & aura : this->auras) aura->BeforeRemoved();
				this->auras.clear();
			}

		public: // hooks
			void AfterMinionSummoned();

		private:
			std::list<Aura*> auras;
		};

	} // namespace BoardObjects
} // namespace GameEngine