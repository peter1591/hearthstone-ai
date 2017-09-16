#pragma once

#include <assert.h>
#include <vector>
#include <random>

#include "Cards/id-map.h"

namespace ui
{
	namespace board
	{
		class UnknownCardsSet
		{
		public:
			UnknownCardsSet(std::vector<Cards::CardId> const& cards)
				: cards_(cards), cards_size_(0)
			{}

			void Add(Cards::CardId card_id) { cards_.push_back(card_id); }
			void Remove(Cards::CardId card_id) {
				for (auto it = cards_.begin(); it != cards_.end(); ++it) {
					if (*it == card_id) {
						cards_.erase(it);
						return;
					}
				}
				// TOOD: wrong deck guess, what to do?
			}

			void ResetState()
			{
				cards_size_ = cards_.size();
			}

			Cards::CardId RandomGet(std::mt19937 & rand)
			{
				assert(cards_size_ > 0);
				size_t idx = (size_t)(rand() % cards_size_);
				
				--cards_size_;
				std::swap(cards_[cards_size_], cards_[idx]);

				return cards_[cards_size_];
			}

			template <class Functor>
			void ForEachRestCard(Functor && functor) const {
				for (size_t i = 0; i < cards_size_; ++i) {
					functor(cards_[i]);
				}
			}

		private:
			std::vector<Cards::CardId> cards_;
			size_t cards_size_;
		};
		
		class UnknownCardsSets
		{
		private:
			struct SetItem
			{
				SetItem(std::vector<Cards::CardId> const& cards) : cards_(cards) {}

				UnknownCardsSet cards_;
				size_t ref_cards_; // how many cards drawn from set
			};

		public:
			size_t AddCardsSet(std::vector<Cards::CardId> const& cards)
			{
				SetItem new_item(cards);
				new_item.ref_cards_ = 0;

				size_t idx = sets_.size();
				sets_.push_back(new_item);
				return idx;
			}

			size_t AssignCardToSet(size_t set_idx)
			{
				assert(set_idx < sets_.size());
				size_t idx = sets_[set_idx].ref_cards_;
				++sets_[set_idx].ref_cards_;
				return idx;
			}

			void RemoveCardFromSet(size_t set_idx, Cards::CardId card_id) {
				sets_[set_idx].cards_.Remove(card_id);
			}

			void Reset() {
				sets_.clear();
			}
			void ResetState()
			{
				for (auto & set : sets_) {
					set.cards_.ResetState();
				}
			}

			template <class Functor>
			void ForEach(Functor && functor) const
			{
				for (auto & set : sets_) {
					functor(set.cards_, set.ref_cards_);
				}
			}

		private:
			std::vector<SetItem> sets_;
		};

		class UnknownCardsSetsManager
		{
		public:
			UnknownCardsSetsManager(UnknownCardsSets & data) : data_(data), shuffled_cards_() {}

			void Prepare(std::mt19937 & rand)
			{
				data_.ResetState();
				
				std::vector<Cards::CardId> cards_pool;
				data_.ForEach([&](UnknownCardsSet const& set, size_t ref_cards) {
					set.ForEachRestCard([&](Cards::CardId card_id) {
						cards_pool.push_back(card_id);
					});

					shuffled_cards_.emplace_back();
					for (size_t i = 0; i < ref_cards; ++i) {
						assert(!cards_pool.empty());
						int rand_idx = rand() % cards_pool.size();
						std::swap(cards_pool[rand_idx], cards_pool.back());
						shuffled_cards_.back().push_back(cards_pool.back());
						cards_pool.pop_back();
					}
				});
			}

			Cards::CardId GetCardId(size_t set_idx, size_t card_idx) const
			{
				return shuffled_cards_[set_idx][card_idx];
			}

		private:
			UnknownCardsSets & data_; // TODO: should use const& ideally
			std::vector<std::vector<Cards::CardId>> shuffled_cards_;
		};
	}
}