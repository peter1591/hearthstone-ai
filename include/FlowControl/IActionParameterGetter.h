#pragma once

#include <vector>
#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "Cards/id-map.h"

namespace state {
	class State;
}

namespace FlowControl {
	class IActionParameterGetter
	{
	public:
		IActionParameterGetter() {}
		IActionParameterGetter(IActionParameterGetter const&) = delete;
		IActionParameterGetter & operator=(IActionParameterGetter const&) = delete;

		virtual int GetMinionPutLocation(int min, int max) = 0;

		virtual state::CardRef GetDefender(std::vector<state::CardRef> const& targets) = 0;

		// spell target
		virtual state::CardRef GetSpecifiedTarget(
			state::State & state, state::CardRef card_ref,
			std::vector<state::CardRef> const& targets) = 0;

		virtual size_t ChooseOne(std::vector<Cards::CardId> const& cards) = 0;
	};
}