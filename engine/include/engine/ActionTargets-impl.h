#pragma once

#include "engine/ActionTargets.h"
#include "engine/FlowControl/ValidActionGetter.h"

namespace engine {
	inline void ActionTargets::Fill(state::PlayerSide side, FlowControl::ValidActionGetter const& game) {
		card_refs_[ActionTargetIndices::GetIndexForHero(side)] = game.GetHeroRef(side);

		int idx = ActionTargetIndices::GetIndexForMinion(side, 0);
		game.ForEachMinion(side, [&](state::CardRef card_ref) {
			assert(ActionTargetIndices::ParseHero(idx) == false);
			card_refs_[idx] = card_ref;
			++idx;
			return true;
		});
	}
}