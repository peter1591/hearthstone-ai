#pragma once

#include "board.h"
#include "player.h"

namespace GameEngine
{
	inline Player::Player(Board & board) : hand(board.random_generator), hero(board), minions(board)
	{

	}

	inline Player::Player(Board & board, Player const & rhs) :
		stat(rhs.stat),
		secrets(rhs.secrets),
		hand(board.random_generator, rhs.hand),
		hero(board, rhs.hero),
		minions(board, rhs.minions)
	{
	}

	inline Player::Player(Board & board, Player && rhs) :
		stat(std::move(rhs.stat)),
		secrets(std::move(rhs.secrets)),
		hand(board.random_generator, std::move(rhs.hand)),
		hero(board, std::move(rhs.hero)),
		minions(board, std::move(rhs.minions))
	{
	}

	inline Player & Player::operator=(Player && rhs)
	{
		this->stat = std::move(rhs.stat);
		this->secrets = std::move(rhs.secrets);
		this->hand = std::move(rhs.hand);
		this->hero = std::move(rhs.hero);
		this->minions = std::move(rhs.minions);
		return *this;
	}

	inline bool Player::operator==(Player const& rhs) const
	{
		if (this->stat != rhs.stat) return false;
		if (this->hand != rhs.hand) return false;
		if (this->secrets != rhs.secrets) return false;
		if (this->hero != rhs.hero) return false;
		if (this->minions != rhs.minions) return false;
		return true;
	}

	inline bool Player::operator!=(Player const& rhs) const
	{
		return !(*this == rhs);
	}

} // namespace GameEngine