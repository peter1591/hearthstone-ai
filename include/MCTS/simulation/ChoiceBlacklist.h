#pragma once

#include <assert.h>
#include <memory>
#include <algorithm>

#include "MCTS/simulation/TreeNode.h"

namespace mcts
{
	namespace simulation
	{
		// Some (sub-)action results in an invalid game state
		// Ideally, we should prevent this situation as much as possible
		// However, for performance consideration and code simplicity,
		//    we don't want to do so many pre-checking, so
		//    sometimes a (sub-)action might still yields an invalid game state
		// In this case, we want to record the path of (sub-)actions yielding an invalid state
		//    and prevent this path for a following re-trial
		// Optimization:
		//    In a native design, a tree might be used to record the black-list choice
		//    of each choice.
		//    However, since we guarantee that when we retrying an invalid choice,
		//    all sub-actions except THE LAST ONE will be chosen, we can use a linear
		//    data structure to record the black list choices, rather than using a tree.
		// TODO
		//    1. Maybe most of the case leads to a valid state. Can we write
		//       to the vector only when an invalid state is detected?
		class ChoiceBlacklist
		{
		private:
			struct Item {
				// Blacklist only cares about manually-chosen actions
				// From which, the maximum chosen set is to choose a target,
				// which is with at most 16 choices
				static constexpr size_t kMaxChoices = 16;

				TreeNodeAddon addon_;
				detail::NodeIndexMap valid_indics_;
				int choice_;

				Item() : addon_(), valid_indics_(), choice_(-1) {}
			};

		public:
			ChoiceBlacklist() : items_(), idx_(0) {}

			void Reset() {
				items_.clear();
				idx_ = 0;
			}

			void FillChoices(int choices) {
				assert(choices >= 0);
				if (idx_ < items_.size()) {
					return;
				}

				auto& item = items_.emplace_back();
				for (size_t i = 0; i < (size_t)choices; ++i) {
					item.valid_indics_.PushBack(i);
				}
			}

			bool IsValid(int choice) const {
				bool is_valid = false;
				ForEachWhiteListItem([&](int valid_choice) {
					if (choice == valid_choice) {
						is_valid = true;
						return false; // early stop
					}
					return true;
				});
				return is_valid;
			}

			TreeNodeAddon & GetCurrentNodeAddon() {
				assert(idx_ < items_.size());
				return items_[idx_].addon_;
			}

			size_t GetWhiteListCount() const {
				assert(idx_ < items_.size());
				return items_[idx_].valid_indics_.Size();
			}
			int GetWhiteListItem(size_t idx) const {
				assert(idx_ < items_.size());
				return (int)items_[idx_].valid_indics_.Get(idx);
			}
			template <typename Functor>
			void ForEachWhiteListItem(Functor&& functor) const {
				assert(idx_ < items_.size());
				return items_[idx_].valid_indics_.ForEach([&](size_t idx) {
					return functor((int)idx);
				});
			}

			void ApplyChoice(int choice) {
				assert(idx_ < items_.size());
				items_[idx_].choice_ = choice;
				++idx_;
			}

			void ReportInvalidChoice() {
				assert(!items_.empty());
				assert(idx_ == items_.size());

				while (true) {
					int choice = items_.back().choice_;
					bool erased = items_.back().valid_indics_.Erase(choice);
					(void)erased;
					assert(erased);

					if (!items_.back().valid_indics_.Empty()) {
						// still has other valid choices, leave it alone
						break;
					}

					// cascade mark parent choice as invalid if no valid choice left
					items_.pop_back();
					assert(!items_.empty()); // at least one valid action in main action
				}
			}

			void Restart() {
				idx_ = 0;
			}

		private:
			std::vector<Item> items_;
			size_t idx_;
		};
	}
}