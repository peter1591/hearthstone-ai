#pragma once

#include <array>

#include "state/Types.h"
#include "engine/FlowControl/ActionTargetIndex.h"

namespace engine
{
	namespace FlowControl
	{
		class ValidActionGetter;
	}

	class ActionTargets
	{
	public:
		void Analyze(FlowControl::ValidActionGetter const& game) {
			// TODO: no need to reset entries for release builds
			for (auto& item : card_refs_) item.Invalidate();

			Fill(state::kPlayerFirst, game);
			Fill(state::kPlayerSecond, game);
		}

		state::CardRef GetCardRef(int target_index) const {
			return card_refs_[target_index];
		}

	private:
		void Fill(state::PlayerSide side, FlowControl::ValidActionGetter const& game);

	private:
		std::array<state::CardRef, FlowControl::ActionTargetIndex::kMaxValue> card_refs_;
	};
}