#pragma once

#include <array>
#include "state/Types.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Deck
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

		public:
			Deck() : change_id_(0), size_(0) {}

			int Size() const { return size_; }
			bool Empty() const { return size_ == 0; }
			int GetLast() const { return cards_[size_ - 1]; }
			int GetChangeId() const { return change_id_; }

			template <typename RandomGenerator>
			void ShuffleAdd(int card_id, RandomGenerator&& rand)
			{
				assert(size_ < max_size);

				++change_id_;
				cards_[size_] = card_id;
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
			std::array<int, max_size> cards_; // key: card id
		};
	}
}