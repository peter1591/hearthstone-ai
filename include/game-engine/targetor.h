#pragma once

#include <bitset>

#define TARGETOR_PLAYER_HERO           0
#define TARGETOR_PLAYER_MINION_START   1
#define TARGETOR_OPPONENT_HERO         8
#define TARGETOR_OPPONENT_MINION_START 9
#define TARGETOR_MAX                   16

class Targetor
{
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

	static int GetOpponentMinionIndex(int pos) { return TARGETOR_OPPONENT_MINION_START + pos; }
	static bool IsOpponentMinion(int const pos, int &minion_idx)
	{
		minion_idx = pos - TARGETOR_OPPONENT_MINION_START;
		if (minion_idx < 0 || minion_idx >= 8) return false;
		return true;
	}
};

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
