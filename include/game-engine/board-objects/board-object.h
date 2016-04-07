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
		explicit BoardObject(Hero & hero)
		{
			this->ptr = &hero;
		}

		explicit BoardObject(Minion & minion)
		{
			this->ptr = &minion;
		}

		bool IsHero() const
		{
			return dynamic_cast<Hero*>(ptr) != nullptr;
		}
		bool IsMinion() const
		{
			return dynamic_cast<Minion*>(ptr) != nullptr;
		}

		Hero & AsHero() const
		{
			auto p = dynamic_cast<Hero*>(ptr);
			if (p) return *p;
			else throw std::runtime_error("type not match");
		}

		Minion & AsMinion() const
		{
			auto p = dynamic_cast<Minion*>(ptr);
			if (p) return *p;
			else throw std::runtime_error("type not match");
		}

		ObjectBase * operator->() const { return this->ptr; }

	private:
		ObjectBase * ptr;
	};

} // namespace GameEngine