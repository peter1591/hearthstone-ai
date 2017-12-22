#pragma once

#include "engine/IActionParameterGetter.h"
#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/PlayerStateView.h"

namespace engine
{
	inline void IActionParameterGetter::Initialize(state::State const& game_state) {
		analyzer_.Analyze(FlowControl::ValidActionGetter(game_state));
	}

	inline void IActionParameterGetter::Initialize(FlowControl::CurrentPlayerStateView const& board) {
		analyzer_.Analyze(board.GetValidActionGetter());
	}
}