#pragma once

#include <bitset>

namespace GameEngine {
	class Board;

namespace BoardObjects {
	class MinionConstIteratorWithSlotIndex;
}

	enum SlotIndex {
		SLOT_INVALID = -1,
		SLOT_PLAYER_SIDE = 0,
		SLOT_PLAYER_HERO = SLOT_PLAYER_SIDE,
		SLOT_PLAYER_MINION_START = 1,
		SLOT_OPPONENT_SIDE = 8,
		SLOT_OPPONENT_HERO = SLOT_OPPONENT_SIDE,
		SLOT_OPPONENT_MINION_START = 9,
		SLOT_MAX = 16
	};

	class SlotIndexBitmap
	{
	public:
		static constexpr int bitset_size = SLOT_MAX;

		void SetOneTarget(SlotIndex idx) { this->bitmap.set(idx); }

		bool None() const { return this->bitmap.none(); }
		int Count() const { return (int)this->bitmap.count(); }

		SlotIndex GetOneTarget() const
		{
			for (int i = 0; i < bitset_size; ++i) {
				if (this->bitmap[i]) return (SlotIndex)i;
			}
			throw std::runtime_error("no target available");
		}

		void ClearOneTarget(SlotIndex idx) { this->bitmap.set(idx, false); }
		void Clear() { this->bitmap.reset(); }

		bool operator==(SlotIndexBitmap const& rhs) const {
			return this->bitmap == rhs.bitmap;
		}

		bool operator!=(SlotIndexBitmap const& rhs) const {
			return !(*this == rhs);
		}

	private:
		std::bitset<bitset_size> bitmap;
	};

	class SlotIndexHelper
	{
	public:
		static void Increase(SlotIndex &idx) { idx = (SlotIndex)((int)idx + 1); }

		static SlotIndex GetMinionIndex(SlotIndex side, int minion_idx) {
			if (side == SLOT_PLAYER_SIDE) return GetPlayerMinionIndex(minion_idx);
			else if (side == SLOT_OPPONENT_SIDE) return GetOpponentMinionIndex(minion_idx);
			else throw std::runtime_error("invalid argument");
		}
		static SlotIndex GetPlayerMinionIndex(int minion_idx) { return (SlotIndex)(SLOT_PLAYER_MINION_START + minion_idx); }
		static SlotIndex GetOpponentMinionIndex(int minion_idx) { return (SlotIndex)(SLOT_OPPONENT_MINION_START + minion_idx); }

		static bool IsPlayerSide(SlotIndex idx)
		{
			if (idx < SLOT_PLAYER_HERO) {
				throw std::runtime_error("invalid argument");
			}
			else if (idx < SLOT_OPPONENT_HERO) {
				return true; // player's side
			}
			else if (idx < SLOT_MAX) {
				return false; // opponent's side
			}
			else {
				throw std::runtime_error("invalid argument");
			}
		}

		static bool IsHero(SlotIndex idx)
		{
			if (idx < SLOT_PLAYER_HERO || idx >= SLOT_MAX) {
				throw std::runtime_error("invalid argument");
			}

			if (idx == SLOT_PLAYER_HERO || idx == SLOT_OPPONENT_HERO) return true;
			else return false;
		}

		static bool IsMinion(SlotIndex idx) { return !IsHero(idx); }

		static bool IsOpponentSide(SlotIndex idx) { return !IsPlayerSide(idx); }

		static SlotIndex GetSide(SlotIndex idx)
		{
			if (IsPlayerSide(idx)) return SLOT_PLAYER_SIDE;
			else return SLOT_OPPONENT_SIDE;
		}

		enum TargetType {
			TARGET_TYPE_PLAYER_ATTACKABLE,
			TARGET_TYPE_OPPONENT_ATTACKABLE,

			TARGET_TYPE_PLAYER_CAN_BE_ATTACKED,
			TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED,

			TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL,
			TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL,

			TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL,
			TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL,

			TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_ENEMY_SPELL,
			TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_ENEMY_SPELL,

			TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL,
			TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL,
		};
		static SlotIndexBitmap GetTargets(TargetType type, GameEngine::Board const& board);

	private:
		static bool MarkAttackableMinions(Board const& board, SlotIndex side, SlotIndexBitmap & bitmap);
		static bool MarkTauntMinions(Board const& board, SlotIndex side, SlotIndexBitmap & bitmap);
		static bool MarkMinionsWithoutStealth(Board const& board, SlotIndex side, SlotIndexBitmap & bitmap);
		static bool MarkMinions(Board const& board, SlotIndex side, SlotIndexBitmap & bitmap);
	};
}