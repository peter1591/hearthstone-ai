#include "game-engine/slot-index.h"
#include "game-engine/board.h"

namespace GameEngine {

	// return true if any
	inline bool SlotIndexHelper::MarkAttackableMinions(BoardObjects::MinionConstIteratorWithSlotIndex && minion_iterator, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (; !minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator->Attackable()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkTauntMinions(BoardObjects::MinionConstIteratorWithSlotIndex && minion_iterator, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (; !minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator->stat.IsTaunt() && !minion_iterator->stat.IsStealth()) { // taunt with stealth will not act as taunt
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinionsWithoutStealth(BoardObjects::MinionConstIteratorWithSlotIndex && minion_iterator, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (; !minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (!minion_iterator->stat.IsStealth()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinions(BoardObjects::MinionConstIteratorWithSlotIndex && minion_iterator, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (; !minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
			ret = true;
		}

		return ret;
	}

	inline SlotIndexBitmap SlotIndexHelper::GetTargets(SlotIndexHelper::TargetType type, GameEngine::Board const & board)
	{
		SlotIndexBitmap targets;

		switch (type)
		{
		case TARGET_TYPE_PLAYER_ATTACKABLE:
			if (board.object_manager.IsPlayerHeroAttackable()) {
				targets.SetOneTarget(SLOT_PLAYER_HERO);
			}
			MarkAttackableMinions(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_OPPONENT_ATTACKABLE:
			if (board.object_manager.IsOpponentHeroAttackable()) {
				targets.SetOneTarget(SLOT_OPPONENT_HERO);
			}
			MarkAttackableMinions(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_PLAYER_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets)) break;

			targets.SetOneTarget(SLOT_PLAYER_HERO);
			MarkMinionsWithoutStealth(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets)) break;

			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			MarkMinionsWithoutStealth(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			targets.SetOneTarget(SLOT_PLAYER_HERO);
			break;

		case TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			break;

		case TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			break;

		case TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board.object_manager.GetPlayerMinionsIteratorWithIndex(), targets);
			targets.SetOneTarget(SLOT_PLAYER_HERO);
			break;

		case TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board.object_manager.GetOpponentMinionsIteratorWithIndex(), targets);
			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}

		return targets;
	}

} // namespace GameEngine