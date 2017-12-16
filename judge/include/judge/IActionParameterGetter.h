#pragma once

// TODO: move this file back to MCTS folder
#include "FlowControl/PlayerStateView.h"
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
			FlowControl::ValidActionGetter(game_state).ForEachPlayableCard([&](size_t idx) {
				playable_cards_.push_back(idx);
				return true;
			});
			FlowControl::ValidActionGetter(game_state).ForEachAttacker([&](int idx) {
				attackers_.push_back(idx);
				return true;
			});
			attacker_indics_ = FlowControl::ValidActionGetter(game_state).GetAttackerIndics();
		}

		void Initialize(FlowControl::CurrentPlayerStateView const& board) {
			playable_cards_.clear();
			board.ForEachPlayableCard([&](size_t idx) {
				playable_cards_.push_back(idx);
				return true;
			});

			attackers_.clear();
			board.ForEachAttacker([&](int idx) {
				attackers_.push_back(idx);
				return true;
			});

			attacker_indics_ = board.GetAttackerIndics();
		}

	public:
		// TODO: also redirect ChooseMainAction() call

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
			assert(!playable_cards_.empty());
			int idx = GetNumber(ActionType::kChooseHandCard, ActionChoices((int)playable_cards_.size()));
			return (int)playable_cards_[idx];
		}

		state::CardRef GetAttacker() final {
			assert(!attackers_.empty());
			int idx = GetNumber(ActionType::kChooseAttacker, (int)attackers_.size());
			return attacker_indics_[attackers_[idx]];
		}

		int GetNumber(ActionType::Types action_type, int exclusive_max) {
			if (exclusive_max <= 0) return -1;
			if (exclusive_max == 1) return 0;
			return GetNumber(action_type, ActionChoices(exclusive_max));
		}

		virtual int GetNumber(ActionType::Types action_type, ActionChoices const& action_choices) = 0;

	private:
		std::vector<size_t> playable_cards_;
		std::array<state::CardRef, 8> attacker_indics_;
		std::vector<int> attackers_;
	};
}