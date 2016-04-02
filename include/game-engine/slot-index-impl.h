#include "game-engine/slot-index.h"
#include "game-engine/board.h"

namespace GameEngine {

	// return true if any
	inline bool SlotIndexHelper::MarkAttackableMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator->Attackable()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkTauntMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator->GetMinion().stat.IsTaunt() && !minion_iterator->GetMinion().stat.IsStealth()) { // taunt with stealth will not act as taunt
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinionsWithoutStealth(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (!minion_iterator->GetMinion().stat.IsStealth()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
			ret = true;
		}

		return ret;
	}

	inline SlotIndexBitmap SlotIndexHelper::GetTargets(SlotIndex side, SlotIndexHelper::TargetType type, GameEngine::Board const & board)
	{
		SlotIndexBitmap targets;

		Player const* friendly_player = nullptr;
		Player const* enemy_player = nullptr;
		SlotIndex friendly_side, enemy_side, friendly_hero, enemy_hero;
		if (side == SlotIndex::SLOT_PLAYER_SIDE) {
			friendly_player = &board.player;
			enemy_player = &board.opponent;
			friendly_side = SlotIndex::SLOT_PLAYER_SIDE;
			enemy_side = SlotIndex::SLOT_OPPONENT_SIDE;
			friendly_hero = SlotIndex::SLOT_PLAYER_HERO;
			enemy_hero = SlotIndex::SLOT_OPPONENT_HERO;
		}
		else if (side == SlotIndex::SLOT_OPPONENT_SIDE) {
			friendly_player = &board.opponent;
			enemy_player = &board.player;
			friendly_side = SlotIndex::SLOT_OPPONENT_SIDE;
			enemy_side = SlotIndex::SLOT_PLAYER_SIDE;
			friendly_hero = SlotIndex::SLOT_OPPONENT_HERO;
			enemy_hero = SlotIndex::SLOT_PLAYER_HERO;
		}
		else {
			throw std::runtime_error("invalid argument");
		}

		switch (type)
		{
		case TARGET_TYPE_ATTACKABLE:
			if (friendly_player->hero.Attackable()) {
				targets.SetOneTarget(friendly_hero);
			}
			MarkAttackableMinions(board, friendly_side, targets);
			break;

		case TARGET_TYPE_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board, friendly_side, targets)) break;

			targets.SetOneTarget(friendly_hero);
			MarkMinionsWithoutStealth(board, friendly_side, targets);
			break;

		case TARGET_TYPE_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, friendly_side, targets);
			break;

		case TARGET_TYPE_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, friendly_side, targets);
			targets.SetOneTarget(friendly_hero);
			break;

		case TARGET_TYPE_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, friendly_side, targets);
			break;

		case TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, friendly_side, targets);
			targets.SetOneTarget(friendly_hero);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}

		return targets;
	}

} // namespace GameEngine