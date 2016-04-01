#pragma once

#include "board.h"
#include "player.h"

namespace GameEngine
{
	inline Player::Player(Board & board) : hand(board.random_generator)
	{

	}

	inline Player::Player(Board & board, Player const & rhs) :
		stat(rhs.stat),
		secrets(rhs.secrets),
		hand(board.random_generator, rhs.hand)
	{
	}

	inline Player::Player(Board & board, Player && rhs) :
		stat(std::move(rhs.stat)),
		secrets(std::move(rhs.secrets)),
		hand(board.random_generator, std::move(rhs.hand))
	{
	}

	inline Player & Player::operator=(Player && rhs)
	{
		this->stat = std::move(rhs.stat);
		this->secrets = std::move(rhs.secrets);
		this->hand = std::move(rhs.hand);
		return *this;
	}

	inline bool Player::operator==(Player const& rhs) const
	{
		if (this->stat != rhs.stat) return false;
		if (this->hand != rhs.hand) return false;
		if (this->secrets != rhs.secrets) return false;
		return true;
	}

	inline bool Player::operator!=(Player const& rhs) const
	{
		return !(*this == rhs);
	}

} // namespace GameEngine