#pragma once

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/IActionParameterGetter.h"
#include "FlowControl/utils/MainOp.h"

namespace FlowControl
{
	namespace utils {
		class ActionApplier
		{
		public:
			class IActionParameterGetter : public FlowControl::IActionParameterGetter
			{
			public:
				virtual MainOpType ChooseMainOp() = 0;
				virtual int ChooseHandCard(std::vector<size_t> const& playable_cards) = 0;
				virtual int	ChooseAttacker(std::vector<int> const& attackers) = 0;
			};

			void Apply(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{

			}
		};
	}
}