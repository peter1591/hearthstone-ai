#pragma once

#include <array>

#include "state/Types.h"

namespace engine
{
	namespace FlowControl
	{
		class ValidActionGetter;
	}

	class ActionTargetIndices
	{
	public:
		static constexpr int kMaxValue = 16;

		// Index definition:
		// FirstPlayer:
		//    Minion 1: 0
		//    Minion 2: 1
		//    ...
		//    Minion 7: 6
		//    Hero:     7
		static constexpr int kPlayerMask = 0x8; // 0 for first player; 1 for second player
		static constexpr int kMinionMask = 0x7;
		static constexpr int kHero = 0x7;

	public:
		static int GetIndexForHero(state::PlayerSide side) {
			int ret = kHero;
			if (side != state::kPlayerFirst) ret |= kPlayerMask;
			return ret;
		}

		static int GetIndexForMinion(state::PlayerSide side, size_t minion_idx) {
			assert(minion_idx < 7);
			int ret = (int)minion_idx;
			if (side != state::kPlayerFirst) ret |= kPlayerMask;
			return ret;
		}

	public:
		static state::PlayerSide ParseSide(int idx) {
			if ((idx & kPlayerMask) == 0) return state::kPlayerSecond;
			else return state::kPlayerFirst;
		}

		static bool ParseHero(int idx) {
			return ((idx & kHero) == kHero);
		}

		static int ParseMinionIndex(int idx) {
			return idx & kHero;
		}
	};

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
		std::array<state::CardRef, ActionTargetIndices::kMaxValue> card_refs_;
	};
}