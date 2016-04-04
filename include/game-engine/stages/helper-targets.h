#pragma once

#include <functional>
#include <list>
#include "game-engine/board.h"
#include "game-engine/player.h"
#include "game-engine/board-objects/board-object.h"

namespace GameEngine
{
	class BoardTarget
	{
	public:
		// TODO: do not use BoardObject (acclerate?)
		BoardTarget(Player & player_, BoardObject const& choosing_object_)
			: player(player_), choosing_object(choosing_object_) {}

		bool IsFriendlyHero() const;
		bool IsEnemyHero() const;
		bool IsFriendlyMinion() const;
		bool IsEnemyMinion() const;

	private:
		Player & player;
		BoardObject choosing_object;
	};

	class BoardTargets
	{
	public:
		typedef std::function<bool(BoardTarget const&)> Decider;

		BoardTargets(Board & board_) : board(board_), has_player_hero(false), has_opponent_hero(false) {}

		BoardTargets(Player & player, Decider decider);
		BoardTargets(Minions & minions);

	public:
		static BoardTargets FriendlyHero(Player & player);
		static BoardTargets EnemyHero(Player & player);
		static BoardTargets BothHero(Player & player);

		static BoardTargets AllMinions(Board & board);

	public: // getters
		Board & GetBoard() const { return this->board; }
		bool HasPlayerHero() const { return this->has_player_hero; }
		bool HasOpponentHero() const { return this->has_opponent_hero; }
		std::list<MinionIterator> const& GetMinionIterators() const { return this->all_minions; }

	private:
		void Decide(Player & player, Player & choosing, Decider decider);
		void Decide(Player & player, MinionIterator const& choosing, Decider decider);
		void Add(Player & player, Player & choosing);
		void Add(MinionIterator const& choosing);

	private:
		Board & board;
		bool has_player_hero;
		bool has_opponent_hero;
		std::list<MinionIterator> all_minions;
	};


} // namespace GameEngine