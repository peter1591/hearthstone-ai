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
#include "board-object.h"

namespace GameEngine {

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