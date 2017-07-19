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
		//    Since most of the time, it should reach a valid state,
		//    we try to do allocations as late as possible until we
		//    met an invalid state.
		// TODO:
		//    If the chance of an invalid state is really low, we can drop the black-list
		//    completely.
		class ChoiceBlacklist
		{
		private:
			struct Item {
				// Blacklist only cares about manually-chosen actions
				// so the max choices is the one of action getter
				static constexpr size_t kMaxChoices = FlowControl::IActionParameterGetter::kMaxChoices;

				TreeNodeAddon addon_;
				detail::NodeIndexMap valid_indics_;
				int choice_;

				Item(int choices) : addon_(), valid_indics_(), choice_(-1) {
					for (size_t i = 0; i < (size_t)choices; ++i) {
						valid_indics_.PushBack(i);
					}
				}
			};

		public:
			ChoiceBlacklist() : items_(), idx_(0), pending_last_choices_(0), pending_last_choice_(0)
			{}

			void Reset() {
				items_.clear();
				idx_ = 0;
				pending_last_choices_ = 0;
				pending_last_choice_ = 0;
			}

			void FillChoices(int choices) {
				assert(choices >= 2); // if there's only one choice, don't callback 
				if (idx_ < items_.size()) {
					if (!items_[idx_].has_value()) {
						items_[idx_] = Item(choices);
					}
					return;
				}

				// delay allocation, just step idx next
				++idx_;
				pending_last_choices_ = choices;
			}

			bool IsValid(int choice) const {
				assert(!HasPending());

				bool is_valid = false;
				ForEachWhiteListItem(choice, [&](int valid_choice) {
					if (choice == valid_choice) {
						is_valid = true;
						return false; // early stop
					}
					return true;
				});
				return is_valid;
			}

			TreeNodeAddon * GetCurrentNodeAddon() {
				if (HasPending()) return nullptr;

				assert(idx_ < items_.size());
				assert(items_[idx_].has_value());
				return &items_[idx_]->addon_;
			}

			size_t GetWhiteListCount(int choices) const {
				if (HasPending()) {
					// pending mode, all choices are still valid
					return choices;
				}

				assert(idx_ < items_.size());
				assert(items_[idx_].has_value());
				return items_[idx_]->valid_indics_.Size();
			}
			int GetWhiteListItem(size_t idx) const {
				if (HasPending()) {
					// pending mode, all choices are still valid
					return (int)idx;
				}

				assert(idx_ < items_.size());
				assert(items_[idx_].has_value());
				return (int)items_[idx_]->valid_indics_.Get(idx);
			}
			template <typename Functor>
			void ForEachWhiteListItem(int choices, Functor&& functor) const {
				if (HasPending()) {
					// pending mode, all choices are still valid
					for (int i = 0; i < choices; ++i) {
						if (!functor(i)) break;
					}
					return;
				}

				assert(idx_ < items_.size());
				assert(items_[idx_].has_value());
				return items_[idx_]->valid_indics_.ForEach([&](size_t idx) {
					return functor((int)idx);
				});
			}

			void ApplyChoice(int choice) {
				if (HasPending()) {
					pending_last_choice_ = choice;
					return;
				}

				assert(idx_ < items_.size());
				assert(items_[idx_].has_value());
				items_[idx_]->choice_ = choice;
				++idx_;
			}

			// @return #-of-steps needed to be ruled-out in next trial.
			//         Normally, this number is one, meaning that
			//         only the last step should be ruled-out, and
			//         all the previous steps should be re-played.
			//         However, if the step before the last step has no
			//         other valid choices, this number will be two.
			//         Meaning that the last two steps should be ruled-out.
			int ReportInvalidChoice() {
				if (idx_ > items_.size()) {
					do {
						items_.emplace_back();
					} while (idx_ > items_.size());
					items_.back() = Item(pending_last_choices_);
					items_.back()->choice_ = pending_last_choice_;
				}

				assert(!items_.empty());
				assert(idx_ == items_.size());

				int invalid_steps = 0;
				while (true) {
					// TODO: use IsInvalidStateBlameNode() to find blame node
					++invalid_steps;
					assert(items_.back().has_value());
					Item & last_item = *items_.back();
					int choice = last_item.choice_;
					bool erased = last_item.valid_indics_.Erase(choice);
					(void)erased;
					assert(erased);

					if (!last_item.valid_indics_.Empty()) {
						// still has other valid choices, leave it alone
						break;
					}

					// cascade mark parent choice as invalid if no valid choice left
					items_.pop_back();
					assert(!items_.empty()); // at least one valid action in main action
				}

				assert(invalid_steps > 0);
				return invalid_steps;
			}

			void Restart() {
				idx_ = 0;
			}

		private:
			bool HasPending() const {
				return idx_ >= items_.size();
			}

		private:
			std::vector<std::optional<Item>> items_;
			size_t idx_;

			int pending_last_choices_;
			int pending_last_choice_;
		};
	}
}