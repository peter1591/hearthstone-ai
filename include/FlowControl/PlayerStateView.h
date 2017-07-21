#pragma once

#include "state/State.h"
#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/FlowController.h"

namespace FlowControl
{
	namespace detail {
		template <state::PlayerSide Side> struct ValidSide {
			static constexpr bool valid = false;
		};

		template <> struct ValidSide<state::kPlayerFirst> {
			static constexpr bool valid = true;
		};
		template <> struct ValidSide<state::kPlayerSecond> {
			static constexpr bool valid = true;
		};
	}

	template <state::PlayerSide Side,
		typename = std::enable_if_t<detail::ValidSide<Side>>>
	class PlayerStateView
	{
	public:
		PlayerStateView(state::State & state) : state_(state) {}

	public: // hand cards
		// Functor parameters: state::Cards::Card const&
		template <typename Functor>
		void ForEachSelfHandCard(Functor && functor) {
			GetPlayer(Side).hand_.ForEach([&](CardRef card_ref) {
				return functor(state_.GetCard(card_ref));
			});
		}

		// Functor parameters:
		//    (TODO) [int] hold_from_turn
		//    (TODO) [bool] enchanted
		template <typename Functor>
		void ForEachOpponentHandCard(Functor && functor) {
			GetOpponentPlayer(Side).hand_.ForEach([&](CardRef card_ref) {
				state::Cards::Card const& card = state_.GetCard(card_ref);
				
				// TODO: implement
				(void)card;

				return functor();
			});
		}

		size_t GetOpponentHandCardCount() {
			return GetOpponentPlayer().hand_.Size();
		}

	public:
		// Functor parameters:
		//    Cards::Card const&
		//    [bool] is_attackable
		template <typename Functor, state::PlayerSide CheckingSide>
		void ForEachMinion(Functor && functor) {
			GetPlayer(CheckingSide).minions_.ForEach([&](state::CardRef card_ref) {
				auto const& card = state_.GetCard(card_ref);
				bool attackable = ValidActionGetter(state_).IsAttackable(card);
				return functor(card, attackable);
			});
		}

		template <state::PlayerSide CheckingSide>
		state::board::PlayerResource const& GetPlayerResource() {
			return GetPlayer(CheckingSide).GetResource();
		}

		template <typename Functor, state::PlayerSide CheckingSide>
		void GetDeckCardCount(Functor && functor) {
			return GetPlayer(CheckingSide).deck_.Size();
		}

		template <state::PlayerSide CheckingSide>
		state::Cards::Card const& GetHeroPower() {
			return state_.GetCard(
				GetPlayer(CheckingSide).GetHeroPowerRef());
		}

		// call Functor only when a weapon is equipped
		template <state::PlayerSide CheckingSide, typename Functor>
		void GetWeapon(Functor && functor) {
			auto ref = GetPlayer(CheckingSide).GetWeaponRef();
			if (ref.IsValid()) functor(state_.GetCard(ref));
		}

		template <state::PlayerSide ViewSide>
		bool IsHeroAttackable() {
			return ValidActionGetter(state_).IsAttackable(
				GetPlayer(ViewSide).GetHeroRef());
		}

		template <state::PlayerSide ViewSide>
		bool IsMinionAttackable(size_t idx) {
			return ValidActionGetter(state_).IsAttackable(
				GetPlayer(ViewSide).minions_.Get(idx));
		}

	private:
		state::board::Player & GetPlayer(state::PlayerIdentifier player) {
			return state_.GetBoard().Get(player)
		}
		state::board::Player & GetPlayer(state::PlayerSide checking_side) {
			return GetPlayer(state::PlayerIdentifier(checking_side))
		}
		state::board::Player & GetOpponentPlayer(state::PlayerSide checking_side) {
			return GetPlayer(state::PlayerIdentifier(checking_side).Opposite())
		}

	private:
		state::State & state_;
	};

	class CurrentPlayerStateView
	{
	public:
		CurrentPlayerStateView(state::State & state) : state_(state) {}

		state::PlayerSide GetCurrentPlayer() const {
			return state_.GetCurrentPlayerId().GetSide();
		}

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

		template <typename Functor>
		static void ApplyWithPlayerStateView(state::State const& state, Functor && functor) {
			state::PlayerSide side = state.GetCurrentPlayerId().GetSide();
			if (side == state::kPlayerFirst) {
				functor(PlayerStateView<state::kPlayerFirst>(state));
			}
			else {
				assert(side == state::kPlayerSecond);
				functor(PlayerStateView<state::kPlayerSecond>(state));
			}
		}

	public: // bridge to flow controller
		template <typename... Args> auto PlayCard(FlowControl::FlowContext & context, Args&&... args) {
			return FlowControl::FlowController(state_, context).PlayCard(std::forward<Args>(args)...);
		}

		auto Attack(FlowControl::FlowContext & context, int attacker_idx) {
			// TODO: move to ValidActionGetter
			state::CardRef attacker;
			if (attacker_idx == 7) {
				attacker = state_.GetCurrentPlayer().GetHeroRef();
			}
			else {
				assert(attacker_idx >= 0);
				assert(attacker_idx < 7);
				attacker = state_.GetCurrentPlayer().minions_.Get((size_t)(attacker_idx));
			}

			assert(attacker.IsValid());
			return FlowControl::FlowController(state_, context).Attack(attacker);
		}

		auto HeroPower(FlowControl::FlowContext & context) {
			return FlowControl::FlowController(state_, context).HeroPower();
		}

		auto EndTurn(FlowControl::FlowContext & context) {
			return FlowControl::FlowController(state_, context).EndTurn();
		}

	private:
		state::State & state_;
	};
}