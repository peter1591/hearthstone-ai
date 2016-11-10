#pragma once

#include <exception>
#include "State/Player.h"
#include "State/PlayerIdentifier.h"

namespace State
{
	class Players
	{
	public:
		Player & Get(PlayerIdentifier identifier)
		{
			if (identifier == kPlayerFirst) return first_;
			else if (identifier == kPlayerSecond) return second_;
			throw std::exception("invalid player identifier");
		}

		const Player & Get(PlayerIdentifier identifier) const
		{
			if (identifier == kPlayerFirst) return first_;
			else if (identifier == kPlayerSecond) return second_;
			throw std::exception("invalid player identifier");
		}

		template <int Identifier> Player & Get();
		template <int Identifier> const Player & Get() const;

	private:
		Player first_;
		Player second_;
	};

	template <> Player & Players::Get<kPlayerFirst>() { return first_; }
	template <> const Player & Players::Get<kPlayerFirst>() const { return first_; }

	template <> Player & Players::Get<kPlayerSecond>() { return second_; }
	template <> const Player & Players::Get<kPlayerSecond>() const { return second_; }
}