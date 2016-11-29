#pragma once

#include <exception>
#include "State/Types.h"
#include "State/board/Player.h"

namespace state
{
	namespace board
	{
		class Board
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

			Player & GetAnother(PlayerIdentifier identifier)
			{
				if (identifier == kPlayerFirst) return second_;
				else if (identifier == kPlayerSecond) return first_;
				throw std::exception("invalid player identifier");
			}

			const Player & GetAnother(PlayerIdentifier identifier) const
			{
				if (identifier == kPlayerFirst) return second_;
				else if (identifier == kPlayerSecond) return first_;
				throw std::exception("invalid player identifier");
			}

			template <int Identifier> Player & Get();
			template <int Identifier> const Player & Get() const;

		private:
			Player first_;
			Player second_;
		};

		template <> Player & Board::Get<kPlayerFirst>() { return first_; }
		template <> const Player & Board::Get<kPlayerFirst>() const { return first_; }

		template <> Player & Board::Get<kPlayerSecond>() { return second_; }
		template <> const Player & Board::Get<kPlayerSecond>() const { return second_; }
	}
}