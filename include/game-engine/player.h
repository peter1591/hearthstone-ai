#pragma once

#include "player-stat.h"
#include "secrets.h"
#include "hand.h"
#include "board-objects/hero.h"
#include "board-objects/minions.h"
#include "game-engine/enchantments/enchantments.h"

namespace GameEngine {

	class Board;

	class Player
	{
		friend std::hash<Player>;

	public:
		Player(Board & board, Player & opposite_player, SlotIndex side);
		Player(Board & board, Player & opposite_player, Player const& rhs);
		Player(Board & board, Player & opposite_player, Player && rhs);
		Player & operator=(Player const& rhs) = delete;
		Player & operator=(Player && rhs);

		bool operator==(Player const& rhs) const;
		bool operator!=(Player const& rhs) const;

		void Destroy();

	public:
		int GetTotalSpellDamage() const;
		Player & GetOppositePlayer() const;
		bool IsPlayerSide() const;

	public:
		Board & board;
		const SlotIndex side;

		Player & opposite_player;
		const SlotIndex opposite_side;

		PlayerStat stat;
		Secrets secrets;
		Hand hand;
		Hero hero;
		Minions minions;

		Enchantments<Player> enchantments;
	};
}

namespace std {
	template <> struct hash<GameEngine::Player> {
		typedef GameEngine::Player argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			// side will not change; don't need for equality comparison

			GameEngine::hash_combine(result, s.stat);
			GameEngine::hash_combine(result, s.secrets);
			GameEngine::hash_combine(result, s.hand);
			GameEngine::hash_combine(result, s.hero);
			GameEngine::hash_combine(result, s.minions);
			GameEngine::hash_combine(result, s.enchantments);

			return result;
		}
	};
}