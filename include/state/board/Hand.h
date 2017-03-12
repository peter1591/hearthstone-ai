#pragma once

#include <assert.h>
#include <vector>
#include "state/Types.h"
#include "state/Cards/Manager.h"

namespace state
{
	namespace board
	{
		class Hand
		{
		public:
			Hand()
			{
				cards_.reserve(max_cards_);
			}

			size_t Size() const { return cards_.size(); }
			bool Empty() const { return cards_.empty(); }
			bool Full() const { return Size() >= max_cards_; }

			CardRef Get(size_t idx) { return cards_[idx]; }

			size_t PushBack(CardRef ref)
			{
				if (cards_.size() >= max_cards_) assert(false);
				size_t ret = cards_.size();
				cards_.push_back(ref);
				return ret;
			}

			template <typename AdjustFunctor>
			void Remove(size_t pos, AdjustFunctor&& functor)
			{
				if (pos >= cards_.size()) throw std::exception("invalid position");

				auto it = cards_.erase(cards_.begin() + pos);
				for (auto it_end = cards_.end(); it != it_end; ++it, ++pos)
				{
					functor(*it, pos);
				}
			}

		private:
			static constexpr int max_cards_ = 10;
			std::vector<CardRef> cards_;
		};
	}
}