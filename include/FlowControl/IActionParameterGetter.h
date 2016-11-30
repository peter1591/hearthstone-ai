#pragma once

#include "State/State.h"

namespace FlowControl
{
	class IActionParameterGetter
	{
	public:
		IActionParameterGetter() {}
		IActionParameterGetter(IActionParameterGetter const&) = delete;
		IActionParameterGetter & operator=(IActionParameterGetter const&) = delete;

		virtual int GetMinionPutLocation(int min, int max) = 0;
		virtual state::CardRef GetBattlecryTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card) = 0;
	};
}