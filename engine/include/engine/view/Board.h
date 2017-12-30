#pragma once

#include "engine/Game.h"
#include "engine/IActionParameterGetter.h"
#include "engine/view/ReducedBoardView.h"

namespace engine
{
	namespace view
	{
		class Board;
		class SavedBoard {
			friend class Board;

		public:
			SavedBoard() : state_() {}
			SavedBoard(SavedBoard const&) = delete;
			SavedBoard & operator=(SavedBoard const&) = delete;

		private:
			SavedBoard(state::State const& state) : state_(state) {}

			state::State state_;
		};

		// Make sure no hidden information is accessed by a player
		class Board
		{
		public:
			Board(engine::Game & game, state::PlayerSide side) :
				game_(game), side_(side)
			{}

		public:
			void RefCopyFrom(Board const& rhs) {
				game_.RefCopyFrom(rhs.game_);
			}

		public:
			state::PlayerIdentifier GetCurrentPlayer() const {
				return game_.GetCurrentState().GetCurrentPlayerId();
			}

			state::PlayerSide GetViewSide() const { return side_; }

			ReducedBoardView CreateView() const {
				if (side_ == state::kPlayerFirst) {
					return ReducedBoardView(engine::view::BoardRefView(
						game_.GetCurrentState(), state::kPlayerFirst));
				}
				else {
					assert(side_ == state::kPlayerSecond);
					return ReducedBoardView(engine::view::BoardRefView(
						game_.GetCurrentState(), state::kPlayerSecond));
				}
			}

			template <class Functor>
			auto ApplyWithPlayerStateView(Functor && functor) const {
				if (side_ == state::kPlayerFirst) {
					return functor(engine::view::BoardRefView(
						game_.GetCurrentState(), state::kPlayerFirst));
				}
				else {
					assert(side_ == state::kPlayerSecond);
					return functor(engine::view::BoardRefView(
						game_.GetCurrentState(), state::kPlayerSecond));
				}
			}

			auto GetCurrentPlayerStateRefView() const {
				if (game_.GetCurrentState().GetCurrentPlayerId().GetSide() != side_) {
					assert(false);
					throw std::runtime_error("current player does not match.");
				}
				return engine::view::CurrentPlayerBoardRefView(game_.GetCurrentState());
			}

		public: // bridge to action analyzer
			engine::Result ApplyAction(engine::IActionParameterGetter & action_parameters) const
			{
				assert(game_.GetCurrentState().GetCurrentPlayerId().GetSide() == side_);
				return game_.PerformAction(action_parameters);
			}

		public:
			void Save(SavedBoard & save) const {
				save.state_ = game_.GetCurrentState();
			}

			void Restore(SavedBoard const& save) {
				game_.SetStartState(save.state_);
			}

		public:
			state::State const& RevealHiddenInformationForSimulation() const {
				// For simulation, it needs state information to estimate who's going to win
				// Hidden information is sometimes a big boost to make a better predict.
				// NOTE: THIS SHOULD ONLY BE USED FOR SIMULATION STATE-VALUE ESTIMATION
				return game_.GetCurrentState();
			}

		private:
			engine::Game & game_;
			state::PlayerSide side_;
		};
	}
}