#pragma once

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/IActionParameterGetter.h"

namespace FlowControl
{
	class ActionApplier
	{
	public:
		class IActionParameterGetter : public FlowControl::IActionParameterGetter
		{
		public:
			virtual int ChooseHandCard(std::vector<size_t> const& playable_cards) = 0;
			virtual int	ChooseAttacker(std::vector<int> const& attackers) = 0;
		};

		void Apply(
			state::State & state,
			FlowControl::FlowContext & flow_context)
		{

		}
	};
}