#pragma once

#include "state/State.h"
#include "engine/FlowControl/ValidActionGetter.h"
#include "engine/FlowControl/FlowController.h"

namespace engine {
	namespace view {
		class BoardRefView
		{
		public:
			BoardRefView(state::State const& state, state::PlayerSide side) :
				state_(state), side_(side)
			{}

			state::PlayerSide GetSide() const { return side_; }

			int GetTurn() const { return state_.GetTurn(); }

		public: // player
			int GetFatigueDamage(state::PlayerSide side) const {
				return state_.GetBoard().Get(side).GetFatigueDamage();
			}

			auto const& GetResource(state::PlayerSide side) const {
				return state_.GetBoard().Get(side).GetResource();
			}

		public: // hero
			state::Cards::Card const& GetSelfHero() const {
				auto ref = GetPlayer(side_).GetHeroRef();
				assert(ref.IsValid());
				return state_.GetCard(ref);
			}

			state::Cards::Card const& GetOpponentHero() const {
				auto ref = GetOpponentPlayer(side_).GetHeroRef();
				assert(ref.IsValid());
				return state_.GetCard(ref);
			}

		public: // hand cards
			// Functor parameters: state::Cards::Card const&
			template <typename Functor>
			void ForEachSelfHandCard(Functor && functor) const {
				GetPlayer(side_).hand_.ForEach([&](state::CardRef card_ref) {
					return functor(state_.GetCard(card_ref));
				});
			}

			// Functor parameters:
			//    (TODO) [int] hold_from_turn
			//    (TODO) [bool] enchanted
			template <typename Functor>
			void ForEachOpponentHandCard(Functor && functor) const {
				GetOpponentPlayer(side_).hand_.ForEach([&](state::CardRef card_ref) {
					state::Cards::Card const& card = state_.GetCard(card_ref);

					// TODO: implement
					(void)card;

					return functor();
				});
			}

			size_t GetOpponentHandCardCount() const {
				return GetOpponentPlayer(side_).hand_.Size();
			}

		public:
			// Functor parameters:
			//    Cards::Card const&
			//    [bool] is_attackable
			template <typename Functor>
			void ForEachMinion(state::PlayerSide view_side, Functor && functor) const {
				GetPlayer(view_side).minions_.ForEach([&](state::CardRef card_ref) {
					auto const& card = state_.GetCard(card_ref);
					bool attackable = engine::FlowControl::ValidActionGetter(state_).IsAttackable(card);
					return functor(card, attackable);
				});
			}

			state::board::PlayerResource const& GetPlayerResource(state::PlayerSide view_side) const {
				return GetPlayer(view_side).GetResource();
			}

			int GetDeckCardCount(state::PlayerSide view_side) const {
				return GetPlayer(view_side).deck_.Size();
			}

			state::Cards::Card const& GetHeroPower(state::PlayerSide view_side) const {
				return state_.GetCard(
					GetPlayer(view_side).GetHeroPowerRef());
			}

			// call Functor only when a weapon is equipped
			template <typename Functor>
			void GetWeapon(state::PlayerSide view_side, Functor && functor) const {
				auto ref = GetPlayer(view_side).GetWeaponRef();
				if (ref.IsValid()) functor(state_.GetCard(ref));
			}

			bool IsHeroAttackable(state::PlayerSide view_side) const {
				return engine::FlowControl::ValidActionGetter(state_).IsAttackable(
					GetPlayer(view_side).GetHeroRef());
			}

			bool IsMinionAttackable(state::PlayerSide view_side, size_t idx) const {
				return engine::FlowControl::ValidActionGetter(state_).IsAttackable(
					GetPlayer(view_side).minions_.Get(idx));
			}

		private:
			state::board::Player const& GetPlayer(state::PlayerIdentifier player) const {
				return state_.GetBoard().Get(player);
			}
			state::board::Player const& GetPlayer(state::PlayerSide checking_side) const {
				return GetPlayer(state::PlayerIdentifier(checking_side));
			}
			state::board::Player const& GetOpponentPlayer(state::PlayerSide checking_side) const {
				return GetPlayer(state::PlayerIdentifier(checking_side).Opposite());
			}

		private:
			state::State const& state_;
			state::PlayerSide side_;
		};

		// Constraint access to the information visible to current player
		class CurrentPlayerBoardRefView
		{
		public:
			CurrentPlayerBoardRefView(state::State const& state) : state_(state) {}

			state::PlayerSide GetCurrentPlayer() const {
				return state_.GetCurrentPlayerId().GetSide();
			}

			auto GetValidActionGetter() const { return engine::FlowControl::ValidActionGetter(state_); }

			template <typename Functor>
			static void ApplyWithCurrentPlayerStateView(state::State const& state, Functor && functor) {
				state::PlayerSide side = state.GetCurrentPlayerId().GetSide();
				if (side == state::kPlayerFirst) {
					functor(BoardRefView(state, state::kPlayerFirst));
				}
				else {
					assert(side == state::kPlayerSecond);
					functor(BoardRefView(state, state::kPlayerSecond));
				}
			}

		private:
			state::State const& state_;
		};
	}
}
