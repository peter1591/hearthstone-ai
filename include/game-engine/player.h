#pragma once

#include "player-stat.h"
#include "secrets.h"
#include "hand.h"
#include "board-objects/hero.h"

namespace GameEngine {

	class Board;

	class Player
	{
		friend std::hash<Player>;

	public:
		Player(Board & board);

		Player(Board & board, Player const& rhs);

		Player(Board & board, Player && rhs);

		Player & operator=(Player const& rhs) = delete;

		Player & operator=(Player && rhs);

		bool operator==(Player const& rhs) const;
		bool operator!=(Player const& rhs) const;

	public:
		PlayerStat stat;
		Secrets secrets;
		Hand hand;
		BoardObjects::Hero hero;
	};
}

namespace std {
	template <> struct hash<GameEngine::Player> {
		typedef GameEngine::Player argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.stat);
			GameEngine::hash_combine(result, s.secrets);
			GameEngine::hash_combine(result, s.hand);
			GameEngine::hash_combine(result, s.hero);

			return result;
		}
	};
}