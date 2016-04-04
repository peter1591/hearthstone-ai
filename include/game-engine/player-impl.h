#pragma once

#include "board.h"
#include "player.h"

namespace GameEngine
{
	inline Player::Player(Board & board, SlotIndex side) :
		board(board), side(side), opposite_side(SlotIndexHelper::GetOppositeSide(side)),
		hand(board.random_generator), hero(board), minions(*this),
		enchantments(*this)
	{

	}

	inline Player::Player(Board & board, Player const & rhs) :
		board(board),
		side(rhs.side), opposite_side(SlotIndexHelper::GetOppositeSide(side)),
		stat(rhs.stat),
		secrets(rhs.secrets),
		hand(board.random_generator, rhs.hand),
		hero(board, rhs.hero),
		minions(*this, rhs.minions),
		enchantments(*this)
	{
#ifdef DEBUG
		if (!rhs.enchantments.Empty()) {
			throw std::runtime_error("you should not copy player with enchantments");
		}
#endif
	}

	inline Player::Player(Board & board, Player && rhs) :
		board(board),
		side(rhs.side), opposite_side(SlotIndexHelper::GetOppositeSide(side)),
		stat(std::move(rhs.stat)),
		secrets(std::move(rhs.secrets)),
		hand(board.random_generator, std::move(rhs.hand)),
		hero(board, std::move(rhs.hero)),
		minions(*this, std::move(rhs.minions)),
		enchantments(*this)
	{
#ifdef DEBUG
		if (!rhs.enchantments.Empty()) {
			throw std::runtime_error("you should not move player with enchantments");
		}
#endif
	}

	inline Player & Player::operator=(Player && rhs)
	{
#ifdef DEBUG
		if (!rhs.enchantments.Empty()) {
			throw std::runtime_error("you should not move player with enchantments");
		}
#endif

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
		if (this->enchantments != rhs.enchantments) return false;
		return true;
	}

	inline bool Player::operator!=(Player const& rhs) const
	{
		return !(*this == rhs);
	}

	inline void Player::Destroy()
	{
		this->minions.Destroy();
	}

} // namespace GameEngine