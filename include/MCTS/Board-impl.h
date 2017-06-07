#pragma once

#include "MCTS/Board.h"
#include "MCTS/ActionParameterGetter.h"
#include "MCTS/RandomGenerator.h"
#include "FlowControl/ValidActionGetter.h"

namespace mcts
{
	inline int Board::GetActionsCount()
	{
		FlowControl::ValidActionGetter valid_action_getter(state_);

		int idx = 0;

		playable_cards_ = valid_action_getter.GetPlayableCards();
		if (!playable_cards_.empty()) {
			actions_[idx++] = &Board::PlayCard;
		}

		attackers_ = valid_action_getter.GetAttackers();
		if (!attackers_.empty()) {
			actions_[idx++] = &Board::Attack;
		}
		
		if (true) { //if (valid_action_getter.HeroPowerUsable()) {
			actions_[idx++] = &Board::HeroPower;
		}

		actions_[idx++] = &Board::EndTurn;

		return idx;
	}

	inline Result Board::ApplyAction(int action, RandomGenerator & random, ActionParameterGetter & action_parameters) {
		return (this->*(actions_[action]))(random, action_parameters);
	}

	inline Result Board::PlayCard(RandomGenerator & random, ActionParameterGetter & action_parameters)
	{
		FlowControl::FlowContext flow_context(random, action_parameters);
		assert(!playable_cards_.empty());
		int idx = action_parameters.GetNumber((int)playable_cards_.size());
		int hand_idx = playable_cards_[idx];
		return FlowControl::FlowController(state_, flow_context).PlayCard(hand_idx);
	}

	inline Result Board::Attack(RandomGenerator & random, ActionParameterGetter & action_parameters)
	{
		FlowControl::FlowContext flow_context(random, action_parameters);
		assert(!attackers_.empty());
		int idx = action_parameters.GetNumber((int)attackers_.size());
		state::CardRef attacker = attackers_[idx];

		return FlowControl::FlowController(state_, flow_context).Attack(attacker);
	}

	inline Result Board::HeroPower(RandomGenerator & random, ActionParameterGetter & action_parameters)
	{
		FlowControl::FlowContext flow_context(random, action_parameters);
		return FlowControl::FlowController(state_, flow_context).HeroPower();
	}

	inline Result Board::EndTurn(RandomGenerator & random, ActionParameterGetter & action_parameters)
	{
		FlowControl::FlowContext flow_context(random, action_parameters);
		return FlowControl::FlowController(state_, flow_context).EndTurn();
	}

	inline state::CardRef Board::UserChooseSideCharacter(state::PlayerIdentifier player_id, ActionParameterGetter & action_parameters)
	{
		int attacker_count = (int)state_.GetBoard().Get(player_id).minions_.Size() + 1;
		int attacker_idx = action_parameters.GetNumber(attacker_count);

		if (attacker_idx == 0) return state_.GetBoard().Get(player_id).GetHeroRef();

		attacker_idx--;
		return state_.GetBoard().Get(player_id).minions_.Get(attacker_idx);
	}
}