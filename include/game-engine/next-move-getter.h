#pragma once

#include <vector>
#include <list>
#include "move.h"
#include "slot-index.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemPlayHandMinion
	{
	public:
		ItemPlayHandMinion(Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemPlayHandMinion* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayHandMinion const& rhs) const;
		bool operator!=(ItemPlayHandMinion const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemPlayHandWeapon
	{
	public:
		ItemPlayHandWeapon(Hand::Locator hand_card, SlotIndexBitmap required_targets);
		ItemPlayHandWeapon* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayHandWeapon const& rhs) const;
		bool operator!=(ItemPlayHandWeapon const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemAttack
	{
	public:
		ItemAttack(SlotIndexBitmap && attacker, SlotIndexBitmap && attacked);
		ItemAttack* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemAttack const& rhs) const;
		bool operator!=(ItemAttack const& rhs) const;
	
	private:
		SlotIndexBitmap attacker;
		SlotIndexBitmap attacked;
		SlotIndexBitmap attacked_origin;
	};

public:
	void AddItem(Move const& move);
	void AddItem(Move && move);
	void AddItem(ItemAttack && items);
	void AddItem(ItemPlayHandMinion && items);
	void AddItem(ItemPlayHandWeapon && items);
	void AddItems(std::list<Move> && items);
	void AddItems(std::list<ItemAttack> && items);
	void AddItems(std::list<ItemPlayHandMinion> && items);
	void AddItems(std::list<ItemPlayHandWeapon> && items);

	bool GetNextMove(Board const& board, Move &move);
	bool Empty();
	void Clear();

	bool operator==(NextMoveGetter const& rhs) const;
	bool operator!=(NextMoveGetter const& rhs) const;

private:
	template <typename T> bool GetNextMoveFromContainer(std::list<T> & container, Board const& board, Move &move);
	template <typename T> static bool IsEqual(std::list<T> const& lhs, std::list<T> const& rhs);

private:
	std::list<Move> moves;
	std::list<ItemAttack> items_player_attack;
	std::list<ItemPlayHandMinion> items_play_hand_minion;
	std::list<ItemPlayHandWeapon> items_play_hand_weapon;
};

} // namespace GameEngine