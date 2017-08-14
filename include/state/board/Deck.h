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
			
		private:
			constexpr static int max_size = 80;
			using CardsContainer = std::array< ::Cards::CardId, max_size>;

		public:
			Deck() : change_id_(0), size_(0), base_cards_(nullptr), cards_() {}

			Deck(Deck const& rhs) :
				change_id_(rhs.change_id_),
				size_(rhs.size_),
				base_cards_(rhs.base_cards_),
				cards_(rhs.cards_)
			{}

			void FillWithBase(Deck const& base) {
				assert(base.base_cards_ == nullptr);

				change_id_ = base.change_id_;
				size_ = base.size_;
				base_cards_ = &base.cards_;
			}

			int Size() const { return size_; }
			bool Empty() const { return size_ == 0; }
			::Cards::CardId GetLast() const { return GetCard(size_ - 1); }
			int GetChangeId() const { return change_id_; }

			void ShuffleAdd(::Cards::CardId card_id, IRandomGenerator & random)
			{
				PrepareForWrite();

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
			// @return kInvalidCardId if no card left; otherwise, the card id of the chosen random card
			::Cards::CardId GetOneRandomCard(IRandomGenerator & random) {
				if (size_ <= 0) return ::Cards::kInvalidCardId;

				int rand_idx = 0;
				if (size_ > 1) rand_idx = random.Get(size_);
				return GetCard(rand_idx);
			}

			std::pair< ::Cards::CardId, ::Cards::CardId> GetTwoRandomCards(IRandomGenerator & random) {
				if (size_ < 2) return std::make_pair(GetOneRandomCard(random), ::Cards::kInvalidCardId);

				int v1 = random.Get(size_);
				int v2 = random.Get(size_ - 1);
				if (v2 >= v1) ++v2;
				return std::make_pair(
					GetCard(v1),
					GetCard(v2));
			}

			template <typename Functor>
			void ForEach(Functor const& functor) {
				auto const& container = GetCards();
				for (int i = size_ - 1; i >= 0; --i) {
					if (!functor(container[i])) return;
				}
			}

			// @return  True if card found; false if card is not found
			bool SwapCardIdToLast(::Cards::CardId card_id) {
				PrepareForWrite();

				for (int i = 0; i < size_; ++i) {
					if (cards_[i] == card_id) {
						std::swap(cards_[size_ - 1], cards_[i]);
						return true;
					}
				}
				return false;
			}

		private:
			::Cards::CardId GetCard(size_t idx) const {
				return GetCards()[idx];
			}

			CardsContainer const& GetCards() const {
				if (base_cards_) return *base_cards_;
				else return cards_;
			}

			void PrepareForWrite() {
				if (base_cards_) {
					cards_ = *base_cards_;
					base_cards_ = nullptr;
				}
			}

		private:
			int change_id_;
			int size_;

			// Copy-On-Write:
			// If base_cards_ is nullptr, use cards_
			// If base_cards_ is not a nullptr, use base_cards_ for copy-on-write
			CardsContainer const* base_cards_;
			CardsContainer cards_;
		};
	}
}