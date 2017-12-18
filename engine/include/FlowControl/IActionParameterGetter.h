#pragma once

#include <vector>
#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "Cards/id-map.h"
#include "FlowControl/MainOp.h"
#include "FlowControl/ActionType.h"
#include "FlowControl/ActionChoices.h"
#include "FlowControl/ValidActionAnalyzer.h"

namespace state {
	class State;
}

namespace FlowControl {
	class IActionParameterGetterWithoutAnalyzer
	{
	public:
		IActionParameterGetterWithoutAnalyzer() {}
		virtual ~IActionParameterGetterWithoutAnalyzer() {}
		IActionParameterGetterWithoutAnalyzer(IActionParameterGetterWithoutAnalyzer const&) = delete;
		IActionParameterGetterWithoutAnalyzer & operator=(IActionParameterGetterWithoutAnalyzer const&) = delete;

		static constexpr size_t kMaxChoices = 16; // max #-of-choices: choose target

		virtual MainOpType ChooseMainOp() = 0;

		// TODO: should we return size_t?
		virtual int ChooseHandCard() = 0;

		virtual state::CardRef GetAttacker() = 0;
		virtual state::CardRef GetDefender(std::vector<state::CardRef> const& targets) = 0;

		virtual int GetMinionPutLocation(int minions) = 0;

		// spell target
		virtual state::CardRef GetSpecifiedTarget(
			state::State & state, state::CardRef card_ref,
			std::vector<state::CardRef> const& targets) = 0;

		virtual Cards::CardId ChooseOne(std::vector<Cards::CardId> const& cards) = 0;
	};

	class CurrentPlayerStateView;
	class IActionParameterGetter : public IActionParameterGetterWithoutAnalyzer
	{
	public:
		void Initialize(state::State const& game_state);
		void Initialize(CurrentPlayerStateView const& board);

		auto const& GetAnalyzer() { return analyzer_; }

	public:
		MainOpType ChooseMainOp() final
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