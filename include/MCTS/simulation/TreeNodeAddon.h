#pragma once

#include <optional>

#include "MCTS/Types.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	namespace simulation
	{
		class TreeNodeActionChoiceChecker
		{
		public:
			TreeNodeActionChoiceChecker() : item_() {}

			bool Check(ActionType action_type, board::ActionChoices action_choices) {
				if (!item_.has_value()) {
					item_ = Item{action_type, action_choices};
					return true;
				}

				if (item_->action_type != action_type) return false;
				if (!board::ActionChoices::HasSameChoices(item_->action_choices, action_choices)) return false;
				return true;
			}

			void Clear() {
				item_.reset();
			}

		private:
			struct Item {
				ActionType action_type;
				board::ActionChoices action_choices;
			};
			std::optional<Item> item_;
		};

		struct TreeNodeAddon
		{
			TreeNodeAddon() :
				action_choice_checker()
			{}

			void Clear() {
				action_choice_checker.Clear();
			}

			TreeNodeActionChoiceChecker action_choice_checker; // TODO: debug check only
		};
	}
}