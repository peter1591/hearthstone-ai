#pragma once

#include "state/State.h"
#include "state/targetor/TargetsGenerator.h"
#include "Cards/id-map.h"
#include "Cards/Contexts.h"

namespace Cards
{
	class MinionCardUtils
	{
	public:
		template <typename Context>
		class DamageHelper
		{
		public:
			template <typename T>
			DamageHelper(T&& context) : context_(std::forward<T>(context)) {}

			DamageHelper & Target(state::CardRef target) {
				assert(!target_.IsValid());
				target_ = target;
				return *this;
			}
			DamageHelper & Owner() {
				assert(!target_.IsValid());
				target_ = context_.manipulate_.Board().Player(context_.card_.GetPlayerIdentifier()).GetHeroRef();
				return *this;
			}
			DamageHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.manipulate_.Board().Player(context_.card_.GetPlayerIdentifier().Opposite()).GetHeroRef();
				return *this;
			}

			void Amount(int amount) {
				assert(target_.IsValid());
				context_.manipulate_.Character(target_)
					.Damage(context_.card_ref_, amount);
			}

		private:
			state::CardRef target_;
			Context & context_;
		};

	public:
		template <typename Context>
		static FlowControl::Manipulate Manipulate(Context&& context)
		{
			return context.manipulate_;
		}

		template <typename Context>
		static FlowControl::Manipulators::Helpers::DeathrattlesHelper Deathrattles(Context&& context)
		{
			return Manipulate(context).Minion(context.card_ref_).Deathrattles();
		}

		template <typename Context>
		static DamageHelper<std::decay_t<Context>> DamageA(Context&& context)
		{
			return DamageHelper<std::decay_t<Context>>(std::forward<Context>(context));
		}

		static state::targetor::TargetsGenerator Targets(state::PlayerIdentifier targeting_player) {
			return state::targetor::TargetsGenerator(targeting_player);
		}

		template <typename Context>
		static state::PlayerIdentifier OwnedPlayer(Context&& context)
		{
			return context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
		}

		template <typename Context>
		static state::CardRef Owner(Context&& context)
		{
			return context.state_.GetBoard().Get(OwnedPlayer(context)).hero_ref_;
		}

		template <typename Context>
		static state::board::Player& Player(Context&& context, state::PlayerIdentifier player)
		{
			return context.manipulate_.Board().Player(player);
		}

		template <typename Context>
		static state::board::Player& Player(Context&& context)
		{
			return Player(std::forward<Context>(context), OwnedPlayer(context));
		}

		template <typename Context>
		static state::board::Player& AnotherPlayer(Context&& context)
		{
			return Player(std::forward<Context>(context), OwnedPlayer(context).Opposite());
		}

		static bool IsMortallyWounded(state::Cards::Card const& target)
		{
			if (target.GetHP() <= 0) return true;
			return false;
		}

		static bool IsInPlay(state::Cards::Card const& target)
		{
			if (target.GetZone() == state::kCardZonePlay) return true;
			return false;
		}

	public:
		template <typename Context, typename Functor>
		static void ForEach(
			Context&& context,
			state::targetor::TargetsGenerator_Leaf const& targets_generator,
			Functor&& func)
		{
			targets_generator.GetInfo().ForEach(context.manipulate_, func);
		}
	};
}