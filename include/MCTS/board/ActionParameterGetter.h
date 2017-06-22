#pragma once

#include "MCTS/ActionType.h"
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
				return targets[GetNumber(ActionType::kChooseDefender, (int)size)];
			}

			// Inclusive min & max
			int GetMinionPutLocation(int min, int max) final
			{
				assert(max >= min);
				return min + GetNumber(ActionType::kChooseMinionPutLocation, max - min + 1);
			}

			state::CardRef GetSpecifiedTarget(
				state::State & state, state::CardRef card_ref,
				std::vector<state::CardRef> const& targets) final
			{
				if (targets.empty()) return state::CardRef();
				size_t size = targets.size();
				return targets[GetNumber(ActionType::kChooseTarget, (int)size)];
			}

			size_t ChooseOne(std::vector<Cards::CardId> const& cards) final
			{
				assert(!cards.empty());
				size_t size = cards.size();
				return (size_t)GetNumber(ActionType::kChooseOne, (int)size);
			}

			int GetNumber(ActionType::Types action_type, int exclusive_max);

		private:
			builder::TreeBuilder & builder_;
		};
	}
}