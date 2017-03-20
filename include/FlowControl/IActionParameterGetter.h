#pragma once

#include <vector>
#include "state/Types.h"
#include "state/State.h"

namespace FlowControl {
	class IActionParameterGetter
	{
	public:
		IActionParameterGetter() {}
		IActionParameterGetter(IActionParameterGetter const&) = delete;
		IActionParameterGetter & operator=(IActionParameterGetter const&) = delete;

		virtual int GetMinionPutLocation(int min, int max) = 0;
		virtual state::CardRef GetBattlecryTarget( // TODO: rename to 'GetSpecifiedTarget'
			state::State & state, state::CardRef card_ref, const state::Cards::Card & card,
			std::vector<state::CardRef> const& targets) = 0;
	};
}