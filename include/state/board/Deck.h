#pragma once

#include <array>
#include <utility>

#include "Cards/id-map.h"
#include "state/Types.h"
#include "state/IRandomGenerator.h"

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
			::Cards::CardId GetLast() const { return cards_[size_ - 1]; }
			int GetChangeId() const { return change_id_; }

			void ShuffleAdd(::Cards::CardId card_id, IRandomGenerator & random)
			{
				assert(size_ < max_size);

				++change_id_;
				cards_[size_] = card_id;
				++size_;

				if (size_ <= 1) return;
				auto rand_idx = random.Get(size_);
				std::swap(cards_[rand_idx], cards_[size_ - 1]);
			}

			void RemoveLast()
			{
				++change_id_;
				--size_;
			}

			// @note This differs from GetLast() since you cannot remove this random card.
			// @return -1 if no card left; otherwise, the card id of the chosen random card
			::Cards::CardId GetOneRandomCard(IRandomGenerator & random) {
				if (size_ <= 0) return (::Cards::CardId)-1;

				int rand_idx = 0;
				if (size_ > 1) rand_idx = random.Get(size_);
				return cards_[rand_idx];
			}

			std::pair<::Cards::CardId, ::Cards::CardId> GetTwoRandomCards(IRandomGenerator & random) {
				if (size_ < 2) return std::make_pair(GetOneRandomCard(random), (::Cards::CardId)-1);

				int v1 = random.Get(size_);
				int v2 = random.Get(size_ - 1);
				if (v2 >= v1) ++v2;
				return std::make_pair(
					cards_[v1],
					cards_[v2]);
			}

			template <typename Functor>
			void ForEach(Functor const& functor) {
				for (int i = size_ - 1; i >= 0; --i) {
					if (!functor(cards_[i])) return;
				}
			}

			// @return  True if card found; false if card is not found
			bool SwapCardIdToLast(::Cards::CardId card_id) {
				for (int i = 0; i < size_; ++i) {
					if (cards_[i] == card_id) {
						std::swap(cards_[size_ - 1], cards_[i]);
						return true;
					}
				}
				return false;
			}

		private:
			constexpr static int max_size = 80;
			int change_id_;
			int size_;
			std::array<::Cards::CardId, max_size> cards_; // key: card id. TODO: change type to Cards::CardId
		};
	}
}