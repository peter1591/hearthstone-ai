#pragma once

#include "engine/view/Board.h"

namespace mcts {
	namespace policy {
		class SideController {
		public:
			class Side {
			public:
				static constexpr Side First() { return Side(state::kPlayerFirst); }
				static constexpr Side Second() { return Side(state::kPlayerSecond); }

				explicit constexpr Side(state::PlayerSide side) : v_(side) {}

				bool operator==(Side const& rhs) const { return v_ == rhs.v_; }
				bool operator!=(Side const& rhs) const { return v_ != rhs.v_; }

				Side Opposite() const { 
					return Side(state::OppositePlayerSide(v_));
				}
				bool IsFirst() const { return v_ == state::kPlayerFirst; }
				bool IsSecond() const { return v_ == state::kPlayerSecond; }

				state::PlayerSide GetStateSide() const { return v_; }

			private:
				state::PlayerSide v_;
			};

		public:
			SideController() : game_() {}

			template <class StartBoardGetter>
			void StartEpisode(StartBoardGetter&& start_board_getter) {
				game_.SetStartState(start_board_getter());
			}

			Side GetActionSide() {
				return Side(game_.GetCurrentState().GetCurrentPlayerId().GetSide());
			}

			auto GetSideView(Side side) {
				return engine::view::Board(game_, side.GetStateSide());
			}

		private:
			engine::Game game_;
		};
	}
}
