#pragma once

#include "engine/IActionParameterGetter.h"
#include "engine/FlowControl/ValidActionGetter.h"
#include "engine/PlayerStateView.h"

namespace engine
{
	inline void IActionParameterGetter::Initialize(state::State const& game_state) {
		analyzer_.Analyze(FlowControl::ValidActionGetter(game_state));
	}

	inline void IActionParameterGetter::Initialize(engine::CurrentPlayerStateView const& board) {
		analyzer_.Analyze(board.GetValidActionGetter());
	}
}