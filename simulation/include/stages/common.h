#ifndef STAGES_COMMON_H
#define STAGES_COMMON_H

// enable consistency checks? (introduce performance drops)
#define ENABLE_DEBUG_CHECKS

// options
#define CHOOSE_WHERE_TO_PUT_MINION

#include <string>
#include <vector>
#include "board.h"
#include "stages/common.h"

class StageCommonUtilities
{
	public:
		static void GetGameFlowMove(std::vector<Move> &next_moves)
		{
			Move move;
			move.action = Move::ACTION_GAME_FLOW;
			next_moves.push_back(move);
		}
};

namespace StageFunctionChooser
{
	struct Chooser_IsPlayerTurn {
		typedef bool ReturnType;
	};

	struct Chooser_IsRandomNode {
		typedef bool ReturnType;
	};

	struct Chooser_ApplyMove {
		typedef void ReturnType;
	};

	struct Chooser_GetNextMoves {
		typedef void ReturnType;
	};

	struct Chooser_GetStageStringName {
		typedef std::string ReturnType;
	};

	template <typename Chooser> struct Caller {};

	template<> struct Caller<Chooser_IsPlayerTurn> {
		public:
			template <typename Stage>
			static Chooser_IsPlayerTurn::ReturnType Call() { return Stage::is_player_turn; }
	};

	template<> struct Caller<Chooser_IsRandomNode> {
		public:
			template <typename Stage>
			static Chooser_IsRandomNode::ReturnType Call() { return Stage::is_random_node; }
	};

	template<> struct Caller<Chooser_ApplyMove> {
		public:
			template <typename Stage, typename... Params>
			static Chooser_ApplyMove::ReturnType Call(Params & ... params) { return Stage::ApplyMove(params...); }
	};

	template<> struct Caller<Chooser_GetNextMoves> {
		public:
			template <typename Stage, typename... Params>
			static Chooser_ApplyMove::ReturnType Call(Params & ... params) { return Stage::GetNextMoves(params...); }
	};

	template<> struct Caller<Chooser_GetStageStringName> {
		public:
			template <typename Stage, typename... Params>
			static Chooser_GetStageStringName::ReturnType Call(Params & ... params) { return Stage::GetStageStringName(params...); }
	};
}

#endif
