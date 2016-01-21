#ifndef STAGES_COMMON_H
#define STAGES_COMMON_H

#include <typeinfo>
#include <string>
#include <vector>
#include "board.h"

#include "stages/helper.h"

namespace GameEngine {

namespace StageFunctionChooser
{
	template <typename Chooser> struct Caller {};

	struct Chooser_GetStageStringName {
		typedef std::string ReturnType;
	};
	template<> struct Caller<Chooser_GetStageStringName> {
		template <typename Stage, typename... Params>
			static Chooser_GetStageStringName::ReturnType Call(Params & ...) {
				return typeid(Stage).name();
			}
	};

	struct Chooser_GetNextMoves {
		typedef void ReturnType;
	};

	template <bool IsGameFlowStage>
	struct CallerInternal_GetNextMoves{};

	// IsGameFlowStage = true
	// A game flow stage's GetNextMoves() always return the game-flow move,
	// So this call should be avoided for perfomance issue
	template<> struct CallerInternal_GetNextMoves<true> {
		template <typename Stage, typename... Params>
		static Chooser_GetNextMoves::ReturnType Call(Params & ...) {
			throw std::runtime_error("This is a game flow stage, the GetNextMoves() call should be avoided.");
		}
	};

	// IsGameFlowStage = false
	template<> struct CallerInternal_GetNextMoves<false> {
		template <typename Stage, typename... Params>
		static Chooser_GetNextMoves::ReturnType Call(Params & ... params) {
			return Stage::GetNextMoves(params...);
		}
	};

	template<> struct Caller<Chooser_GetNextMoves> {
		public:
			template <typename Stage, typename... Params>
			static Chooser_GetNextMoves::ReturnType Call(Params & ... params) {
				constexpr bool is_game_flow_stage =
					((Stage::stage & STAGE_TYPE_FLAG) == STAGE_TYPE_GAME_FLOW);

				return CallerInternal_GetNextMoves<is_game_flow_stage>::template Call<Stage, Params...>(params...);
			}

		private:

	};

	struct Chooser_ApplyMove {
		typedef void ReturnType;
	};

	template <bool IsGameFlowStage>
	struct CallerInternal_ApplyMove{};

	// IsGameFlowStage = true
	// A game flow stage can only apply a game-flow move,
	// and the function name is Go()
	template<> struct CallerInternal_ApplyMove<true> {
		template <typename Stage, typename... Params>
		static Chooser_ApplyMove::ReturnType Call(Board &board, const Move &move) {
#ifdef DEBUG
			if (move.action != Move::ACTION_GAME_FLOW) throw std::runtime_error("Invalid move");
#else
			(void)move;
#endif
			return Stage::Go(board);
		}
	};

	// IsGameFlowStage = false
	template<> struct CallerInternal_ApplyMove<false> {
		template <typename Stage, typename... Params>
		static Chooser_ApplyMove::ReturnType Call(Params & ... params)
		{
			return Stage::ApplyMove(params...);
		}
	};

	template<> struct Caller<Chooser_ApplyMove> {
		template <typename Stage, typename... Params>
			static Chooser_ApplyMove::ReturnType Call(Params & ... params)
			{
				constexpr bool is_game_flow_stage =
					((Stage::stage & STAGE_TYPE_FLAG) == STAGE_TYPE_GAME_FLOW);

				return CallerInternal_ApplyMove<is_game_flow_stage>::template Call<Stage, Params...>(params...);
			}
	};

} // namespace StageFunctionChooser

} // namespace GameEngine

#endif
