#pragma once

#include <vector>
#include <list>
#include "move.h"
#include "slot-index.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemPlayerPlayMinion
	{
	public:
		ItemPlayerPlayMinion(Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemPlayerPlayMinion* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayerPlayMinion const& rhs) const;
		bool operator!=(ItemPlayerPlayMinion const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemOpponentPlayMinion
	{
	public:
		ItemOpponentPlayMinion(Card card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemOpponentPlayMinion* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemOpponentPlayMinion const& rhs) const;
		bool operator!=(ItemOpponentPlayMinion const& rhs) const;

	private:
		Card playing_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemPlayerEquipWeapon
	{
	public:
		ItemPlayerEquipWeapon(Hand::Locator hand_card, SlotIndexBitmap required_targets);
		ItemPlayerEquipWeapon* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayerEquipWeapon const& rhs) const;
		bool operator!=(ItemPlayerEquipWeapon const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemOpponentEquipWeapon
	{
	public:
		ItemOpponentEquipWeapon(Card card, SlotIndexBitmap required_targets);
		ItemOpponentEquipWeapon* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemOpponentEquipWeapon const& rhs) const;
		bool operator!=(ItemOpponentEquipWeapon const& rhs) const;

	private:
		Card playing_card;
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
	void AddItem(ItemPlayerPlayMinion && items);
	void AddItem(ItemOpponentPlayMinion && items);
	void AddItem(ItemPlayerEquipWeapon && items);
	void AddItem(ItemOpponentEquipWeapon && items);
	void AddItems(std::list<Move> && items);
	void AddItems(std::list<ItemAttack> && items);
	void AddItems(std::list<ItemPlayerPlayMinion> && items);
	void AddItems(std::list<ItemOpponentPlayMinion> && items);
	void AddItems(std::list<ItemPlayerEquipWeapon> && items);
	void AddItems(std::list<ItemOpponentEquipWeapon> && items);

	bool GetNextMove(Board const& board, Move &move);
	bool Empty();

	bool operator==(NextMoveGetter const& rhs) const;
	bool operator!=(NextMoveGetter const& rhs) const;

private:
	template <typename T> bool GetNextMoveFromContainer(std::list<T> & container, Board const& board, Move &move);
	template <typename T> static bool IsEqual(std::list<T> const& lhs, std::list<T> const& rhs);

private:
	std::list<Move> moves;
	std::list<ItemAttack> items_player_attack;
	std::list<ItemPlayerPlayMinion> items_player_play_minion;
	std::list<ItemOpponentPlayMinion> items_opponent_play_minion;
	std::list<ItemPlayerEquipWeapon> items_player_equip_weapon;
	std::list<ItemOpponentEquipWeapon> items_opponent_equip_weapon;
};

} // namespace GameEngine