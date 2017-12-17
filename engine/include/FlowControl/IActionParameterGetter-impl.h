#pragma once

#include "FlowControl/IActionParameterGetter.h"
#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/PlayerStateView.h"

namespace FlowControl
{
	inline void IActionParameterGetter::Initialize(state::State const& game_state) {
		analyzer_.Analyze(ValidActionGetter(game_state));
	}

	inline void IActionParameterGetter::Initialize(CurrentPlayerStateView const& board) {
		analyzer_.Analyze(board.GetValidActionGetter());
	}
}