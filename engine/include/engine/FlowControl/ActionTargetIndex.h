#pragma once

#include "state/Types.h"

namespace engine {
	namespace FlowControl {
		class ActionTargetIndex
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
	}
}