#pragma once

// TODO: move this file back to MCTS folder
#include "FlowControl/PlayerStateView.h"
#include "FlowControl/ValidActionAnalyzer.h"
#include "MCTS/Types.h"
#include "MCTS/board/ActionChoices.h"

namespace judge
{
	class IActionParameterGetter : public FlowControl::IActionParameterGetter
	{
	public:
		using ActionType = mcts::ActionType; // TODO: remove this
		using ActionChoices = mcts::board::ActionChoices; // TODO: remove this

	public:
		void Initialize(state::State const& game_state) {
			analyzer_.Analyze(FlowControl::ValidActionGetter(game_state));
		}

		void Initialize(FlowControl::CurrentPlayerStateView const& board) {
			analyzer_.Analyze(board.GetValidActionGetter());
		}

	public:
		FlowControl::MainOpType ChooseMainOp() final
		{
			auto main_ops_count = analyzer_.GetMainActionsCount();
			auto const& main_ops = analyzer_.GetMainActions();
			int main_op_idx = GetNumber(ActionType::kMainAction, main_ops_count);
			return main_ops[main_op_idx];
		}

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

		int ChooseHandCard() final {
			auto const& playable_cards = analyzer_.GetPlayableCards();
			assert(!playable_cards.empty());
			int idx = GetNumber(ActionType::kChooseHandCard, ActionChoices((int)playable_cards.size()));
			return (int)playable_cards[idx];
		}

		state::CardRef GetAttacker() final {
			auto const& attackers = analyzer_.GetAttackers();
			auto const& attacker_indics = analyzer_.GetAttackerIndics();
			assert(!attackers.empty());
			int idx = GetNumber(ActionType::kChooseAttacker, (int)attackers.size());
			return attacker_indics[attackers[idx]];
		}

		int GetNumber(ActionType::Types action_type, int exclusive_max) {
			if (exclusive_max <= 0) return -1;
			if (exclusive_max == 1) return 0;
			return GetNumber(action_type, ActionChoices(exclusive_max));
		}

		virtual int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) = 0;

	protected:
		FlowControl::ValidActionAnalyzer analyzer_;
	};
}