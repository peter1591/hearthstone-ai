#pragma once

#include "MCTS/Types.h"
#include "FlowControl/IActionParameterGetter.h"

namespace mcts
{
	namespace builder { class TreeBuilder; }

	namespace board
	{
		class ActionParameterGetter : public FlowControl::IActionParameterGetter
		{
		public:
			ActionParameterGetter(builder::TreeBuilder & builder) : builder_(builder) {}

			state::CardRef GetDefender(std::vector<state::CardRef> const& targets) final
			{
				assert(!targets.empty());
				size_t size = targets.size();
				int idx = GetNumber(ActionType::kChooseDefender, (int)size);
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
				size_t size = targets.size();
				int idx = GetNumber(ActionType::kChooseTarget, (int)size);
				assert(idx >= 0 && idx < size);
				return targets[idx];
			}

			size_t ChooseOne(std::vector<Cards::CardId> const& cards) final
			{
				assert(!cards.empty());
				size_t size = cards.size();
				int idx = GetNumber(ActionType::kChooseOne, (int)size);
				assert(idx >= 0 && idx < size);
				return (size_t)idx;
			}

			int GetNumber(ActionType::Types action_type, int exclusive_max);

		private:
			builder::TreeBuilder & builder_;
		};
	}
}