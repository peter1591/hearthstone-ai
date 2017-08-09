#pragma once

#include "FlowControl/IActionParameterGetter.h"
#include "MCTS/Types.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class IActionParameterGetter : public FlowControl::IActionParameterGetter
		{
		public:
			state::CardRef GetDefender(std::vector<state::CardRef> const& targets) final
			{
				assert(!targets.empty());
				int size = (int)targets.size();
				int idx = GetNumber(ActionType::kChooseDefender, size);
				assert(idx >= 0 && idx < size);
				return targets[idx];
			}

			// Inclusive min & max
			int GetMinionPutLocation(int minions) final
			{
				assert(minions >= 0);
				int v = GetNumber(ActionType::kChooseMinionPutLocation, minions + 1);
				assert(v >= 0 && v <= minions);
				return v;
			}

			state::CardRef GetSpecifiedTarget(
				state::State & state, state::CardRef card_ref,
				std::vector<state::CardRef> const& targets) final
			{
				if (targets.empty()) return state::CardRef();
				int size = (int)targets.size();
				int idx = GetNumber(ActionType::kChooseTarget, size);
				assert(idx >= 0 && idx < size);
				return targets[idx];
			}

			Cards::CardId ChooseOne(std::vector<Cards::CardId> const& cards) final
			{
				assert(!cards.empty());
				return (Cards::CardId)GetNumber(ActionType::kChooseOne, ActionChoices(cards));
			}

			int GetNumber(ActionType::Types action_type, int exclusive_max) {
				if (exclusive_max <= 0) return -1;
				if (exclusive_max == 1) return 0;
				return GetNumber(action_type, ActionChoices(exclusive_max));
			}

			virtual int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) = 0;
		};

		class ActionParameterGetter : public IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : callback_(callback) {}

			int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) final;

		private:
			SOMCTS & callback_;
		};
	}
}