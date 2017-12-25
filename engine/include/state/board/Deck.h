#pragma once

#include <array>
#include <utility>

#include "Cards/id-map.h"
#include "state/Types.h"

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

			void RefCopy(Deck const& base) {
				assert(base.base_cards_ == nullptr);

				change_id_ = base.change_id_;
				size_ = base.size_;
				base_cards_ = &base.cards_;
			}

			int Size() const { return size_; }
			bool Empty() const { return size_ == 0; }
			::Cards::CardId GetLast() const { return GetCard(size_ - 1); }
			int GetChangeId() const { return change_id_; }

			template <class RandomGetter>
			void ShuffleAdd(::Cards::CardId card_id, RandomGetter && random_getter)
			{
				PrepareForWrite();

				assert(size_ < max_size);
				assert(card_id != ::Cards::kInvalidCardId);

				++change_id_;
				cards_[size_] = card_id;
				++size_;

				if (size_ <= 1) return;
				auto rand_idx = random_getter(size_);
				std::swap(cards_[rand_idx], cards_[size_ - 1]);
			}

			void RemoveLast()
			{
				++change_id_;
				--size_;
			}

			template <typename Functor>
			void ForEach(Functor const& functor) const {
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

			::Cards::CardId GetCard(size_t idx) const {
				return GetCards()[idx];
			}

		private:
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