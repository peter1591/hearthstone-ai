#pragma once

#include "helper-targets.h"

namespace GameEngine
{
	inline bool BoardTarget::IsFriendlyHero() const
	{
		if (!this->choosing_object.IsHero()) return false;
		if (&this->choosing_object.AsHero() != &this->player.hero) return false;
		return true;
	}

	inline bool BoardTarget::IsEnemyHero() const
	{
		if (!this->choosing_object.IsHero()) return false;
		if (&this->choosing_object.AsHero() == &this->player.hero) return false;
		return true;
	}

	inline bool BoardTarget::IsFriendlyMinion() const
	{
		if (!this->choosing_object.IsMinion()) return false;
		if (&this->choosing_object.AsMinion().GetMinions() != &this->player.minions) return false;
		return true;
	}

	inline bool BoardTarget::IsEnemyMinion() const
	{
		if (!this->choosing_object.IsMinion()) return false;
		if (&this->choosing_object.AsMinion().GetMinions() == &this->player.minions) return false;
		return true;
	}

	inline BoardTargets::BoardTargets(Player & player, Decider decider) : BoardTargets(player.board)
	{
		this->Decide(player, this->board.player, decider);
		this->Decide(player, this->board.opponent, decider);

		for (auto it = this->board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			this->Decide(player, it, decider);
		}
		for (auto it = this->board.opponent.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
			this->Decide(player, it, decider);
		}
	}

	inline BoardTargets::BoardTargets(Minions & minions) : BoardTargets(minions.GetBoard())
	{
		for (auto it = this->board.player.minions.GetIterator(0); !it.IsEnd(); it.GoToNext()) {
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

	inline BoardTargets BoardTargets::FriendlyCharacters(Player & player)
	{
		BoardTargets ret(player.minions);
		if (player.IsPlayerSide()) ret.has_player_hero = true;
		else ret.has_opponent_hero = true;
		return ret;
	}

	inline BoardTargets BoardTargets::EnemyCharacters(Player & player)
	{
		BoardTargets ret(player.opposite_player.minions);
		if (player.IsPlayerSide()) ret.has_opponent_hero = true;
		else ret.has_player_hero = true;
		return ret;
	}

	inline BoardTargets BoardTargets::AllCharacters(Board & board)
	{
		BoardTargets ret = BoardTargets::AllMinions(board);
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

	inline BoardObject BoardTargets::GetOneRandomValidTarget() const
	{
		std::vector<std::list<MinionIterator>::const_iterator> valid_idx_map;
		valid_idx_map.reserve(this->all_minions.size());

		int count = 0;
		for (auto it = this->all_minions.cbegin(); it != this->all_minions.cend(); ++it)
		{
			if ((*it)->GetHP() <= 0) continue;
			valid_idx_map.push_back(it);
			++count;
		}

		bool has_player = this->has_player_hero;
		if (has_player) {
			if (this->board.player.hero.GetHP() <= 0) has_player = false;
		}

		bool has_opponent = this->has_opponent_hero;
		if (has_opponent) {
			if (this->board.opponent.hero.GetHP() <= 0) has_opponent = false;
		}

		if (has_player) ++count;
		if (has_opponent) ++count;

		if (count == 0) throw std::out_of_range("no valid target");

		int r = this->board.random_generator.GetRandom(count);

		if (has_player) {
			if (r == 0) return BoardObject(this->board.player.hero);
			--r;
		}

		if (has_opponent) {
			if (r == 0) return BoardObject(this->board.opponent.hero);
			--r;
		}

		return BoardObject(**valid_idx_map[r]);
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