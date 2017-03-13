#pragma once

#include <algorithm>
#include <array>
#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/Cards/Card.h"

namespace state
{
	namespace board
	{
		class Deck
		{
		public:
			Deck() : change_id_(0), size_(0) {}

			CardRef GetLast() const { return cards_[size_-1]; }

			int Size() const { return size_; }
			bool Empty() const { return size_ == 0; }

			template <typename RandomGenerator>
			void ShuffleAdd(CardRef card, RandomGenerator&& rand)
			{
				assert(size_ < max_size);

				++change_id_;
				cards_[size_] = card;
				++size_;

				if (size_ <= 1) return;
				auto rand_idx = rand(size_);
				std::swap(cards_[rand_idx], cards_[size_ - 1]);
			}

			void RemoveLast()
			{
				++change_id_;
				--size_;
			}

		private:
			constexpr static int max_size = 80;
			int change_id_;
			int size_;
			std::array<CardRef, max_size> cards_;
		};
	}
}