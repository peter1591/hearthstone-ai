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

	inline void SlotIndexHelper::SetTargets(SlotIndex side, SideTargetType type, GameEngine::Board const & board, SlotIndexBitmap & targets)
	{
		Player const* player = nullptr;
		SlotIndex hero;
		if (side == SlotIndex::SLOT_PLAYER_SIDE) {
			player = &board.player;
			hero = SlotIndex::SLOT_PLAYER_HERO;
		}
		else if (side == SlotIndex::SLOT_OPPONENT_SIDE) {
			player = &board.opponent;
			hero = SlotIndex::SLOT_OPPONENT_HERO;
		}
		else {
			throw std::runtime_error("invalid argument");
		}

		switch (type)
		{
		case TARGET_TYPE_ATTACKABLE:
			if (player->hero.Attackable()) {
				targets.SetOneTarget(hero);
			}
			MarkAttackableMinions(board, side, targets);
			break;

		case TARGET_TYPE_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board, side, targets)) break;

			targets.SetOneTarget(hero);
			MarkMinionsWithoutStealth(board, side, targets);
			break;

		case TARGET_TYPE_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, side, targets);
			break;

		case TARGET_TYPE_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, side, targets);
			targets.SetOneTarget(hero);
			break;

		case TARGET_TYPE_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, side, targets);
			break;

		case TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, side, targets);
			targets.SetOneTarget(hero);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}
	}

	inline SlotIndexBitmap SlotIndexHelper::GetTargets(SlotIndex side, SlotIndexHelper::SideTargetType type, GameEngine::Board const & board)
	{
		SlotIndexBitmap targets;
		SlotIndexHelper::SetTargets(side, type, board, targets);
		return targets;
	}

	inline SlotIndexBitmap SlotIndexHelper::GetTargets(Player const& player, TargetType type)
	{
		SlotIndexBitmap targets;

		switch (type)
		{
		case TARGET_SPELL_ALL_MINIONS:
			SlotIndexHelper::SetTargets(player.side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL, player.board, targets);
			SlotIndexHelper::SetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_ENEMY_SPELL, player.board, targets);
			break;

		case TARGET_SPELL_ALL_CHARACTERS:
			SlotIndexHelper::SetTargets(player.side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL, player.board, targets);
			SlotIndexHelper::SetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, player.board, targets);
			break;

		case TARGET_SPELL_ENEMY_MINIONS:
			SlotIndexHelper::SetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_ENEMY_SPELL, player.board, targets);
			break;

		case TARGET_SPELL_ENEMY_CHARACTERS:
			SlotIndexHelper::SetTargets(player.opposite_side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL, player.board, targets);
			break;

		case TARGET_SPELL_FRIENDLY_MINIONS:
			SlotIndexHelper::SetTargets(player.side, SlotIndexHelper::TARGET_TYPE_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL, player.board, targets);
			break;

		case TARGET_SPELL_FRIENDLY_CHARACTERS:
			SlotIndexHelper::SetTargets(player.side, SlotIndexHelper::TARGET_TYPE_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL, player.board, targets);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}

		return targets;
	}

} // namespace GameEngine