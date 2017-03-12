#pragma once

#include <vector>
#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/Cards/Card.h"

namespace state
{
	namespace board
	{
		class Minions
		{
		public:
			Minions()
			{
				minions_.reserve(7);
			}

			size_t Size() const { return minions_.size(); }
			CardRef Get(size_t pos) const { return minions_[pos]; }
			bool Full() const { return Size() >= 7; }

			std::vector<CardRef> const& Get() const { return minions_; }

			template <typename AdjustFunctor>
			void Insert(CardRef ref, size_t pos, AdjustFunctor&& functor)
			{
				if (pos < 0) throw std::exception("invalid position");
				if (pos > minions_.size()) throw std::exception("invalid position");

				auto it = minions_.insert(minions_.begin() + pos, ref);
				functor(*it, pos);

				++it; ++pos;
				for (auto it_end = minions_.end(); it != it_end; ++it, ++pos) {
					functor(*it, pos);
				}
			}

			template <typename AdjustFunctor>
			void Remove(size_t pos, AdjustFunctor&& functor)
			{
				if (pos >= minions_.size()) throw std::exception("invalid position");

				auto it = minions_.erase(minions_.begin() + pos);
				for (auto it_end = minions_.end(); it != it_end; ++it, ++pos)
				{
					functor(*it, pos);
				}
			}

		private:
			std::vector<CardRef> minions_;
		};
	}
}