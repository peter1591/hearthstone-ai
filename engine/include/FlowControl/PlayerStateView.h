#pragma once

#include "state/State.h"
#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/FlowController.h"

namespace FlowControl
{
	// Constraint access to the information visible to 'Side' player
	template <state::PlayerSide Side,
		typename = std::enable_if_t<state::ValidPlayerSide<Side>::valid>>
	class PlayerStateView
	{
	public:
		PlayerStateView(state::State const& state) : state_(state) {}

		int GetTurn() const { return state_.GetTurn(); }

	public: // hero
		state::Cards::Card const& GetSelfHero() const {
			auto ref = GetPlayer(Side).GetHeroRef();
			assert(ref.IsValid());
			return state_.GetCard(ref);
		}

		state::Cards::Card const& GetOpponentHero() const {
			auto ref = GetOpponentPlayer(Side).GetHeroRef();
			assert(ref.IsValid());
			return state_.GetCard(ref);
		}

	public: // hand cards
		// Functor parameters: state::Cards::Card const&
		template <typename Functor>
		void ForEachSelfHandCard(Functor && functor) const {
			GetPlayer(Side).hand_.ForEach([&](state::CardRef card_ref) {
				return functor(state_.GetCard(card_ref));
			});
		}

		// Functor parameters:
		//    (TODO) [int] hold_from_turn
		//    (TODO) [bool] enchanted
		template <typename Functor>
		void ForEachOpponentHandCard(Functor && functor) const {
			GetOpponentPlayer(Side).hand_.ForEach([&](state::CardRef card_ref) {
				state::Cards::Card const& card = state_.GetCard(card_ref);
				
				// TODO: implement
				(void)card;

				return functor();
			});
		}

		size_t GetOpponentHandCardCount() const {
			return GetOpponentPlayer().hand_.Size();
		}

	public:
		// Functor parameters:
		//    Cards::Card const&
		//    [bool] is_attackable
		template <typename Functor>
		void ForEachMinion(state::PlayerSide view_side, Functor && functor) const {
			GetPlayer(view_side).minions_.ForEach([&](state::CardRef card_ref) {
				auto const& card = state_.GetCard(card_ref);
				bool attackable = ValidActionGetter(state_).IsAttackable(card);
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
			return ValidActionGetter(state_).IsAttackable(
				GetPlayer(view_side).GetHeroRef());
		}

		bool IsMinionAttackable(state::PlayerSide view_side, size_t idx) const {
			return ValidActionGetter(state_).IsAttackable(
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
	};

	// Constraint access to the information visible to current player
	class CurrentPlayerStateView
	{
	public:
		CurrentPlayerStateView(state::State & state) : state_(state) {}

		state::PlayerSide GetCurrentPlayer() const {
			return state_.GetCurrentPlayerId().GetSide();
		}

		auto GetValidActionGetter() const { return ValidActionGetter(state_); }

		template <typename Functor>
		void ForEachPlayableCard(Functor && functor) const {
			ValidActionGetter(state_).ForEachPlayableCard(std::forward<Functor>(functor));
		}

		bool CanUseHeroPower() const {
			return ValidActionGetter(state_).CanUseHeroPower();
		}

		template <typename Functor>
		void ForEachAttacker(Functor&& functor) const {
			return ValidActionGetter(state_).ForEachAttacker(std::forward<Functor>(functor));
		}

		auto GetAttackerIndics() const {
			return ValidActionGetter(state_).GetAttackerIndics();
		}

		template <typename Functor>
		static void ApplyWithCurrentPlayerStateView(state::State const& state, Functor && functor) {
			state::PlayerSide side = state.GetCurrentPlayerId().GetSide();
			if (side == state::kPlayerFirst) {
				functor(PlayerStateView<state::kPlayerFirst>(state));
			}
			else {
				assert(side == state::kPlayerSecond);
				functor(PlayerStateView<state::kPlayerSecond>(state));
			}
		}

	private:
		state::State & state_;
	};
}