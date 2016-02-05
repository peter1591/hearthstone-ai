#pragma once

#include <bitset>

namespace GameEngine {

	class Board;

	class TargetorBitmap
	{
	public:
		static constexpr int bitset_size = 16;

		void SetOneTarget(int idx) { this->bitmap.set(idx); }

		bool None() const { return this->bitmap.none(); }
		int Count() const { return (int)this->bitmap.count(); }

		int GetOneTarget() const
		{
			for (int i = 0; i < bitset_size; ++i) {
				if (this->bitmap[i]) return i;
			}
			throw std::runtime_error("no target available");
		}

		void ClearOneTarget(int idx) { this->bitmap.set(idx, false); }
		void Clear() { this->bitmap.reset(); }

		bool operator==(TargetorBitmap const& rhs) const {
			return this->bitmap == rhs.bitmap;
		}

		bool operator!=(TargetorBitmap const& rhs) const {
			return !(*this == rhs);
		}

	private:
		std::bitset<bitset_size> bitmap;
	};

	class Targetor
	{
	public:
		enum TargetorIndex {
			TARGETOR_PLAYER_HERO = 0,
			TARGETOR_PLAYER_MINION_START = 1,
			TARGETOR_OPPONENT_HERO = 8,
			TARGETOR_OPPONENT_MINION_START = 9,
			TARGETOR_MAX = 16
		};

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

	public:
		static int GetPlayerHeroIndex() { return TARGETOR_PLAYER_HERO; }
		static int GetOpponentHeroIndex() { return TARGETOR_OPPONENT_HERO; }

		static int GetPlayerMinionIndex(int pos) { return TARGETOR_PLAYER_MINION_START + pos; }
		static bool IsPlayerMinion(int const pos, int &minion_idx)
		{
			minion_idx = pos - TARGETOR_PLAYER_MINION_START;
			if (minion_idx < 0 || minion_idx >= 8) return false;
			return true;
		}
		static bool IsPlayerSide(int const pos)
		{
			return (pos >= TARGETOR_PLAYER_HERO && pos < TARGETOR_OPPONENT_HERO);
		}

		static int GetOpponentMinionIndex(int pos) { return TARGETOR_OPPONENT_MINION_START + pos; }
		static bool IsOpponentMinion(int const pos, int &minion_idx)
		{
			minion_idx = pos - TARGETOR_OPPONENT_MINION_START;
			if (minion_idx < 0 || minion_idx >= 8) return false;
			return true;
		}
		static bool IsOpponentSide(int const pos)
		{
			return (pos >= TARGETOR_OPPONENT_HERO && pos < TARGETOR_MAX);
		}

	public: // Generate targetor bitmap
		static TargetorBitmap GetTargets(TargetType type, GameEngine::Board const& board);
	};

}