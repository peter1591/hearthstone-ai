#include "game-engine/targetor.h"
#include "game-engine/minions.h"
#include "game-engine/board.h"

namespace GameEngine {

	// return true if any
	static bool MarkMinionsWithTaunt(GameEngine::Board const& board, GameEngine::Minions const& minions, TargetorBitmap &bitmap)
	{
		bool ret = false;

		for (int i = 0; i < minions.GetMinions().size(); ++i)
		{
			auto const& minion = minions.GetMinions()[i];

			if (minion.IsTaunt() && !minion.IsStealth()) { // taunt with stealth will not act as taunt
				if (&board.player_minions == &minions) bitmap.SetOneTarget(Targetor::GetPlayerMinionIndex(i));
				else if (&board.opponent_minions == &minions) bitmap.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
				else throw std::runtime_error("logic error");
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	// will not mark stealth minions
	static bool MarkMinionsWithoutStealth(GameEngine::Board const& board, GameEngine::Minions const& minions, TargetorBitmap &bitmap)
	{
		bool ret = false;

		for (int i = 0; i < minions.GetMinions().size(); ++i)
		{
			auto const& minion = minions.GetMinions()[i];

			if (!minion.IsStealth()) {
				if (&board.player_minions == &minions) bitmap.SetOneTarget(Targetor::GetPlayerMinionIndex(i));
				else if (&board.opponent_minions == &minions) bitmap.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
				else throw std::runtime_error("logic error");
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	// will not mark stealth minions
	static bool MarkMinions(GameEngine::Board const& board, GameEngine::Minions const& minions, TargetorBitmap &bitmap)
	{
		bool ret = false;

		for (int i = 0; i < minions.GetMinions().size(); ++i)
		{
			if (&board.player_minions == &minions) bitmap.SetOneTarget(Targetor::GetPlayerMinionIndex(i));
			else if (&board.opponent_minions == &minions) bitmap.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
			else throw std::runtime_error("logic error");
			ret = true;
		}

		return ret;
	}

	// return true if any
	static bool MarkMinionsAttackable(GameEngine::Board const& board, GameEngine::Minions const& minions, TargetorBitmap &bitmap)
	{
		bool ret = false;

		for (int i = 0; i < minions.GetMinions().size(); ++i)
		{
			auto const& minion = minions.GetMinions()[i];

			if (minion.Attackable()) {
				if (&board.player_minions == &minions) bitmap.SetOneTarget(Targetor::GetPlayerMinionIndex(i));
				else if (&board.opponent_minions == &minions) bitmap.SetOneTarget(Targetor::GetOpponentMinionIndex(i));
				else throw std::runtime_error("logic error");
				ret = true;
			}
		}

		return ret;
	}

	TargetorBitmap Targetor::GetTargets(Targetor::TargetType type, GameEngine::Board const & board)
	{
		TargetorBitmap targets;

		switch (type)
		{
		case Targetor::TARGET_TYPE_PLAYER_ATTACKABLE:
			// TODO: check if player has attack value
			MarkMinionsAttackable(board, board.player_minions, targets);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_ATTACKABLE:
			// TODO: check if opponent has attack value
			MarkMinionsAttackable(board, board.opponent_minions, targets);
			break;

		case Targetor::TARGET_TYPE_PLAYER_CAN_BE_ATTACKED:
			if (MarkMinionsWithTaunt(board, board.player_minions, targets)) break;
			targets.SetOneTarget(TARGETOR_PLAYER_HERO);
			MarkMinionsWithoutStealth(board, board.player_minions, targets);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED:
			if (MarkMinionsWithTaunt(board, board.opponent_minions, targets)) break;
			targets.SetOneTarget(TARGETOR_OPPONENT_HERO);
			MarkMinionsWithoutStealth(board, board.opponent_minions, targets);
			break;

		case Targetor::TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, board.player_minions, targets);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, board.opponent_minions, targets);
			break;

		case Targetor::TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, board.player_minions, targets);
			targets.SetOneTarget(TARGETOR_PLAYER_HERO);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, board.opponent_minions, targets);
			targets.SetOneTarget(TARGETOR_OPPONENT_HERO);
			break;

		case Targetor::TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, board.player_minions, targets);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, board.opponent_minions, targets);
			break;

		case Targetor::TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, board.player_minions, targets);
			targets.SetOneTarget(TARGETOR_PLAYER_HERO);
			break;

		case Targetor::TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, board.opponent_minions, targets);
			targets.SetOneTarget(TARGETOR_OPPONENT_HERO);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}

		return targets;
	}

} // namespace GameEngine