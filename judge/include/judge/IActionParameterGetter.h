#pragma once

#include "FlowControl/utils/ActionApplier.h"
#include "MCTS/Types.h" // TODO: move to this namespace
#include "MCTS/board/ActionChoices.h" // TODO: move to this namespace

namespace judge
{
	class IActionParameterGetter : public FlowControl::utils::ActionApplier::IActionParameterGetter
	{
	public:
		using ActionType = mcts::ActionType; // TODO: remove this
		using ActionChoices = mcts::board::ActionChoices; // TODO: remove this

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
			assert(cards.size() > 1);
			return (Cards::CardId)GetNumber(ActionType::kChooseOne, ActionChoices(cards));
		}

		int ChooseHandCard(std::vector<size_t> const& playable_cards) final {
			assert(!playable_cards.empty());
			assert(playable_cards.size() > 1);
			return GetNumber(ActionType::kChooseHandCard, ActionChoices((int)playable_cards.size()));
		}

		int ChooseAttacker(std::vector<int> const& attackers) final {
			assert(!attackers.empty());
			assert(attackers.size() > 1);
			return GetNumber(ActionType::kChooseAttacker, (int)attackers.size());
		}

		int GetNumber(ActionType::Types action_type, int exclusive_max) {
			if (exclusive_max <= 0) return -1;
			if (exclusive_max == 1) return 0;
			return GetNumber(action_type, ActionChoices(exclusive_max));
		}

		virtual int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) = 0;
	};
}