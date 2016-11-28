#pragma once

#include "FlowControl/Result.h"
#include "FlowControl/ActionTypes.h"
#include "State/State.h"
#include "FlowControl/Helpers/PlayCard.h"
#include "FlowControl/Helpers/EndTurn.h"
#include "FlowControl/Helpers/Attack.h"
#include "FlowControl/FlowContext.h"

// Implemention details which depends on manipulators
#include "Manipulators/Helpers/OrderedCardsManager-impl.h"
#include "State/Cards/Manager-impl.h"

namespace FlowControl
{
	template <class ActionParameterGetter, class RandomGenerator>
	class FlowController
	{
	public:
		FlowController(state::State & state, ActionParameterGetter & action_parameters, RandomGenerator & random)
			: state_(state), action_parameters_(action_parameters), random_(random)
		{
		}

		Result PlayCard(int hand_idx)
		{
			Helpers::PlayCard<ActionParameterGetter, RandomGenerator> helper(state_, context_, hand_idx, action_parameters_, random_);
			Result rc = helper.Go();
			assert(context_.Empty());
			return rc;
		}

		Result EndTurn()
		{
			Helpers::OnTurnEnd<ActionParameterGetter, RandomGenerator> helper(state_, context_, action_parameters_, random_);
			Result rc = helper.Go();
			assert(context_.Empty());
			return rc;
		}

		Result Attack(state::CardRef attacker, state::CardRef defender)
		{
			Helpers::Attack<ActionParameterGetter, RandomGenerator> helper(state_, context_, attacker, defender, action_parameters_, random_);
			Result rc = helper.Go();
			assert(context_.Empty());
			return rc;
		}

	public:
		state::State & state_;
		ActionParameterGetter & action_parameters_;
		RandomGenerator & random_;
		FlowContext context_;
	};
}