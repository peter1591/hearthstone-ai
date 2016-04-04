#pragma once

#include <vector>
#include <list>
#include "move.h"
#include "slot-index.h"
#include "player.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemPlayHandMinion
	{
	public:
		ItemPlayHandMinion(Player const& player, Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemPlayHandMinion* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayHandMinion const& rhs) const;
		bool operator!=(ItemPlayHandMinion const& rhs) const;

	private:
		Player const& player;
		Hand::Locator hand_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemPlayHandCard // no put location
	{
	public:
		ItemPlayHandCard(Player const& player, Hand::Locator hand_card, SlotIndexBitmap required_targets);
		ItemPlayHandCard* Clone() const;
		bool GetNextMove(Board const& board, Move & move);
		bool operator==(ItemPlayHandCard const& rhs) const;
		bool operator!=(ItemPlayHandCard const& rhs) const;

	private:
		Player const& player;
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
	void AddItem(ItemPlayHandCard && items);
	void AddItems(std::list<Move> && items);
	void AddItems(std::list<ItemAttack> && items);
	void AddItems(std::list<ItemPlayHandMinion> && items);
	void AddItems(std::list<ItemPlayHandCard> && items);

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
	std::list<ItemPlayHandCard> items_play_hand_card;
};

} // namespace GameEngine