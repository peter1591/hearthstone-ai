#pragma once

#include "helper-targets.h"

namespace GameEngine
{
	inline bool BoardTarget::IsFriendlyHero() const
	{
		if (!this->choosing_object.IsHero()) return false;
		if (&this->choosing_object.GetHero() != &this->player.hero) return false;
		return true;
	}

	inline bool BoardTarget::IsEnemyHero() const
	{
		if (!this->choosing_object.IsHero()) return false;
		if (&this->choosing_object.GetHero() == &this->player.hero) return false;
		return true;
	}

	inline bool BoardTarget::IsFriendlyMinion() const
	{
		if (!this->choosing_object.IsMinion()) return false;
		if (&this->choosing_object.GetMinion().GetMinions() != &this->player.minions) return false;
		return true;
	}

	inline bool BoardTarget::IsEnemyMinion() const
	{
		if (!this->choosing_object.IsMinion()) return false;
		if (&this->choosing_object.GetMinion().GetMinions() == &this->player.minions) return false;
		return true;
	}

	inline BoardTargets::BoardTargets(Player & player, Decider decider) : BoardTargets(player.board)
	{
		Board & board = player.board;
		this->Decide(player, board.player, decider);
		this->Decide(player, board.opponent, decider);

		for (auto it = board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			this->Decide(player, it, decider);
		}
		for (auto it = board.opponent.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			this->Decide(player, it, decider);
		}
	}

	inline BoardTargets::BoardTargets(Minions & minions) : BoardTargets(minions.GetBoard())
	{
		Board & board = minions.GetBoard();
		for (auto it = board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			this->Add(it);
		}
	}

	inline BoardTargets BoardTargets::FriendlyHero(Player & player)
	{
		BoardTargets ret(player.board);
		if (&player.board.player == &player) ret.has_player_hero = true;
		else {
#ifdef DEBUG
			if (&player.board.opponent != &player) throw std::runtime_error("consistency check failed");
#endif
			ret.has_opponent_hero = true;
		}
		return ret;
	}

	inline BoardTargets BoardTargets::EnemyHero(Player & player)
	{
		BoardTargets ret(player.board);
		if (&player.board.player == &player) ret.has_opponent_hero = true;
		else {
#ifdef DEBUG
			if (&player.board.opponent != &player) throw std::runtime_error("consistency check failed");
#endif
			ret.has_player_hero = true;
		}
		return ret;
	}

	inline BoardTargets BoardTargets::BothHero(Player & player)
	{
		BoardTargets ret(player.board);
		ret.has_player_hero = true;
		ret.has_opponent_hero = true;
		return ret;
	}

	inline BoardTargets BoardTargets::AllMinions(Board & board)
	{
		BoardTargets ret(board);
		for (auto it = board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			ret.Add(it);
		}
		for (auto it = board.opponent.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			ret.Add(it);
		}
		return ret;
	}

	inline void BoardTargets::Decide(Player & player, Player & choosing, Decider decider)
	{
		if (decider(BoardTarget(player, BoardObject(choosing.hero)))) this->Add(player, choosing);
	}

	inline void BoardTargets::Decide(Player & player, MinionIterator const& choosing, Decider decider)
	{
		if (decider(BoardTarget(player, BoardObject(*choosing)))) this->Add(choosing);
	}

	inline void BoardTargets::Add(Player & player, Player & choosing)
	{
		if (&choosing == &player.board.player) this->has_player_hero = true;
#ifdef DEBUG
		if (&choosing != &player.board.opponent) throw std::runtime_error("consistency check error");
#endif
		this->has_opponent_hero = true;
	}

	inline void BoardTargets::Add(MinionIterator const & choosing)
	{
		this->all_minions.push_back(choosing);
	}
} // namespace GameEngine