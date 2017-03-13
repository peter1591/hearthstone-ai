#pragma once

#include <assert.h>
#include <array>
#include "state/Types.h"
#include "state/Cards/Manager.h"

namespace state
{
	namespace board
	{
		class Hand
		{
		public:
			Hand() : size_(0) {}

			size_t Size() const { return size_; }
			bool Empty() const { return size_ == 0; }
			bool Full() const { return size_ >= max_cards_; }

			CardRef Get(size_t idx) { return cards_[idx]; }

			size_t PushBack(CardRef ref)
			{
				assert(size_ < max_cards_);
				size_t ret = size_;
				cards_[size_] = ref;
				++size_;
				return ret;
			}

			template <typename AdjustFunctor>
			void Remove(size_t pos, AdjustFunctor&& functor)
			{
				assert(pos < size_);

				size_t spot = pos;
				for (size_t i = pos + 1; i < size_; ++i) {
					cards_[spot] = cards_[i];
					functor(cards_[spot], spot);
					++spot;
				}
				--size_;
			}

		private:
			static constexpr int max_cards_ = 10;
			std::array<CardRef, max_cards_> cards_;
			size_t size_;
		};
	}
}