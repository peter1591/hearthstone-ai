#ifndef GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H
#define GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>

#include "game-engine/slot-index.h"

#include "hero-data.h"
#include "minions.h"
#include "minion.h"
#include "hero.h"

namespace GameEngine {
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

class ObjectManager
{
	friend std::hash<ObjectManager>;

public:
	ObjectManager(Board & board);
	~ObjectManager() {}

	ObjectManager(ObjectManager const& rhs) = delete;
	ObjectManager(ObjectManager && rhs) = delete;
	ObjectManager& operator=(ObjectManager const& rhs) = delete;
	ObjectManager& operator=(ObjectManager && rhs) = delete;

	bool operator==(ObjectManager const& rhs) const = delete;
	bool operator!=(ObjectManager const& rhs) const = delete;

public: // Get manipulate object
	BoardObject GetObject(SlotIndex idx);

public: // Manipulate minions
	MinionConstIteratorWithSlotIndex GetMinionsIteratorWithIndexAtBeginOfSide(SlotIndex side) const;

	MinionIterator GetMinionIterator(SlotIndex slot_idx);
	MinionIterator GetMinionIteratorAtBeginOfSide(SlotIndex side);

	Minion & GetMinion(SlotIndex slot_idx);

private:
	Board & board;
};

} // namespace GameEngine
#endif