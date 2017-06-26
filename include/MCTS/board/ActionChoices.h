#pragma once

#include <assert.h>
#include <vector>
#include "Cards/id-map.h"

namespace mcts
{
	namespace board
	{
		class ActionChoices
		{
		public:
			enum Type {
				kInvalid,
				kChooseFromZeroToExclusiveMax,
				kChooseFromCardIds
			};

		public:
			explicit ActionChoices(int exclusive_max) :
				type_(kChooseFromZeroToExclusiveMax),
				exclusive_max_(exclusive_max)
			{}

			explicit ActionChoices(std::vector<Cards::CardId> const& cards) :
				type_(kChooseFromCardIds),
				card_ids_(cards)
			{}

			Type GetType() const { return type_; }

		public: // iterate
			bool Empty() const {
				if (type_ == kChooseFromCardIds) {
					return card_ids_.empty();
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					return exclusive_max_ <= 0;
				}
			}

			int Size() const {
				if (type_ == kChooseFromCardIds) {
					return (int)card_ids_.size();
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					return exclusive_max_;
				}
			}

			void Begin() {
				if (type_ == kChooseFromCardIds) {
					card_ids_it_ = card_ids_.begin();
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					range_it_ = 0;
				}
			}

			int Get() const {
				if (type_ == kChooseFromCardIds) {
					return (int)*card_ids_it_;
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					return range_it_;
				}
			}

			void StepNext() {
				if (type_ == kChooseFromCardIds) {
					++card_ids_it_;
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					++range_it_;
				}
			}

			bool IsEnd() const {
				if (type_ == kChooseFromCardIds) {
					return card_ids_it_ == card_ids_.end();
				}
				else {
					assert(type_ == kChooseFromZeroToExclusiveMax);
					return range_it_ >= exclusive_max_;
				}
			}

		private:
			Type type_;

			int exclusive_max_;

			std::vector<Cards::CardId> card_ids_;

		private: // iterate progress
			int range_it_;
			std::vector<Cards::CardId>::iterator card_ids_it_;
		};
	}
}