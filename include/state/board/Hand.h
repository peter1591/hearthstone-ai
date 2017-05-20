#pragma once

#include <assert.h>
#include <array>
#include "state/Types.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Hand
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

		public:
			Hand() : size_(0), change_id_(0) {}

			size_t Size() const { return size_; }
			bool Empty() const { return size_ == 0; }
			bool Full() const { return size_ >= max_cards_; }
			size_t LeftSpaces() const {
				assert(size_ <= max_cards_);
				return max_cards_ - size_;
			}

			CardRef Get(size_t idx) { return cards_[idx]; }
			int GetChangeId() const { return change_id_; }

			template <typename Functor>
			void ForEach(Functor&& functor) {
				for (int i = 0; i < size_; ++i) {
					if (!functor(cards_[i])) return;
				}
			}

		private:
			size_t PushBack(CardRef ref)
			{
				assert(size_ < max_cards_);
				size_t ret = size_;
				cards_[size_] = ref;
				++size_;
				++change_id_;
				return ret;
			}

			template <typename AdjustFunctor>
			void Remove(size_t pos, AdjustFunctor&& functor)
			{
				assert(pos < size_);

				++change_id_;
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
			int change_id_;
		};
	}
}