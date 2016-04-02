#pragma once

#include "board.h"
#include "player.h"

namespace GameEngine
{
	inline Player::Player(Board & board, SlotIndex side) :
		side(side), hand(board.random_generator), hero(board), minions(board)
	{

	}

	inline Player::Player(Board & board, Player const & rhs) :
		side(rhs.side),
		stat(rhs.stat),
		secrets(rhs.secrets),
		hand(board.random_generator, rhs.hand),
		hero(board, rhs.hero),
		minions(board, rhs.minions)
	{
	}

	inline Player::Player(Board & board, Player && rhs) :
		side(rhs.side),
		stat(std::move(rhs.stat)),
		secrets(std::move(rhs.secrets)),
		hand(board.random_generator, std::move(rhs.hand)),
		hero(board, std::move(rhs.hero)),
		minions(board, std::move(rhs.minions))
	{
	}

	inline Player & Player::operator=(Player && rhs)
	{
		if (this->side != rhs.side) throw std::runtime_error("you should not move across player/opponent");

		this->stat = std::move(rhs.stat);
		this->secrets = std::move(rhs.secrets);
		this->hand = std::move(rhs.hand);
		this->hero = std::move(rhs.hero);
		this->minions = std::move(rhs.minions);
		return *this;
	}

	inline bool Player::operator==(Player const& rhs) const
	{
		// side will not change; don't need to compare

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