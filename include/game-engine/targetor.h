#pragma once

#include <bitset>

#define TARGETOR_PLAYER_HERO           0
#define TARGETOR_PLAYER_MINION_START   1
#define TARGETOR_OPPONENT_HERO         8
#define TARGETOR_OPPONENT_MINION_START 9

class Targetor
{
public:
	static int GetPlayerHeroIndex() { return TARGETOR_PLAYER_HERO; }
	static int GetOpponentHeroIndex() { return TARGETOR_OPPONENT_HERO; }

	static int GetPlayerMinionIndex(int pos) { return TARGETOR_PLAYER_MINION_START + pos; }
	static int GetOpponentMinionIndex(int pos) { return TARGETOR_OPPONENT_MINION_START + pos; }
};

class TargetorBitmap
{
public:
	static constexpr int bitset_size = 16;

	void SetOneTarget(int idx) { this->bitmap.set(idx); }

	bool None() const { return this->bitmap.none(); }
	size_t Count() const { return this->bitmap.count(); }

	size_t GetOneTarget() const
	{
		for (size_t i = 0; i < bitset_size; ++i) {
			if (this->bitmap[i]) return i;
		}
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
