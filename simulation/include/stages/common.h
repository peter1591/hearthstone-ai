#ifndef STAGES_COMMON_H
#define STAGES_COMMON_H

#include <string>
#include <vector>
#include "board.h"
#include "stages/common.h"

namespace StageFunctionChooser
{
	template <typename Chooser> struct Caller {};

	struct Chooser_GetStageStringName {
		typedef std::string ReturnType;
	};
	template<> struct Caller<Chooser_GetStageStringName> {
		template <typename Stage, typename... Params>
			static Chooser_GetStageStringName::ReturnType Call(Params & ... params) { return Stage::GetStageStringName(params...); }
	};

	struct Chooser_GetNextMoves {
		typedef void ReturnType;
	};

	template <bool IsGameFlowStage>
	struct CallerInternal_GetNextMoves{};

	// IsGameFlowStage = true
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

				CallerInternal_GetNextMoves<is_game_flow_stage>::template Call<Stage, Params...>(params...);
			}

		private:

	};

	struct Chooser_ApplyMove {
		typedef void ReturnType;
	};
	template<> struct Caller<Chooser_ApplyMove> {
		template <typename Stage, typename... Params>
			static Chooser_ApplyMove::ReturnType Call(Params & ... params) { return Stage::ApplyMove(params...); }
	};
}

#endif
