#pragma once

#include <stdexcept>
#include "game-engine/board-objects/object-base.h"

namespace GameEngine
{
	class Hero;
	class Minion;

	class BoardObject
	{
	public:
		explicit BoardObject(Hero & hero) : hero(hero), minion(*(Minion*)(nullptr))
		{
			this->ptr = &this->hero;
		}

		explicit BoardObject(Minion & minion) : minion(minion), hero(*(Hero*)(nullptr))
		{
			this->ptr = &this->minion;
		}

		bool IsHero() const { return this->ptr == &this->hero; }
		bool IsMinion() const { return this->ptr == &this->minion; }

		Hero & GetHero()
		{
#ifdef DEBUG
			if (this->IsHero() == false) throw std::runtime_error("type not match.");
#endif
			return this->hero;
		}

		Minion & GetMinion()
		{
#ifdef DEBUG
			if (this->IsMinion() == false) throw std::runtime_error("type not match.");
#endif
			return this->minion;
		}

		ObjectBase * operator->() const { return this->ptr; }

	private:
		Hero & hero;
		Minion & minion;
		ObjectBase * ptr;
	};

} // namespace GameEngine