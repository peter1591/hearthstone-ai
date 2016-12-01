#pragma once

#include "FlowControl/Context/BattleCry.h"
#include "FlowControl/Context/Deathrattle.h"
#include "State/State.h"

namespace Cards
{
	template <typename Context>
	class PlayerHelper
	{
	public:
		template <typename T>
		PlayerHelper(T&& context, state::board::Player & player) : context_(std::forward<T>(context)), player_(player) {}

		void GainEmptyCrystal(int amount = 1)
		{
			player_.resource_.IncreaseTotal(amount);
		}

		void GainCrystal(int amount)
		{
			player_.resource_.IncreaseTotal(amount);
			player_.resource_.IncreaseCurrent(amount);
		}

	private:
		Context & context_;
		state::board::Player & player_;
	};

	class MinionCardBase
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
				target_ = context_.state_.board.Get(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}
			DamageHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.state_.board.GetAnother(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}

			void Amount(int amount) {
				assert(target_.IsValid());
				FlowControl::Manipulate(context_.state_, context_.flow_context_).Character(target_).Damage().Take(amount);
			}

		private:
			state::CardRef target_;
			Context & context_;
		};

	public:
		template <typename Context>
		static FlowControl::Manipulate Manipulate(Context&& context)
		{
			return FlowControl::Manipulate(context.state_, context.flow_context_);
		}

		template <typename Context>
		static FlowControl::Manipulators::Helpers::DeathrattlesHelper Deathrattles(Context&& context)
		{
			return Manipulate(context).Minion(context.card_ref_).Deathrattles();
		}

		template <typename Context>
		static DamageHelper<std::decay_t<Context>> Damage(Context&& context)
		{
			return DamageHelper<std::decay_t<Context>>(std::forward<Context>(context));
		}

		template <typename Context>
		static state::CardRef Owner(Context&& context)
		{
			return context.state_.board.Get(context.card_.GetPlayerIdentifier()).hero_ref_;
		}

		template <typename Context>
		static state::CardRef Opponent(Context&& context)
		{
			return context.state_.board.GetAnother(context.card_.GetPlayerIdentifier()).hero_ref_;
		}

		template <typename Context>
		static PlayerHelper<Context> Player(Context&& context)
		{
			return PlayerHelper<Context>(context, context.state_.board.Get(context.card_.GetPlayerIdentifier()));
		}

		template <typename Context>
		static PlayerHelper<Context> AnotherPlayer(Context&& context)
		{
			return PlayerHelper<Context>(context, context.state_.board.GetAnother(context.card_.GetPlayerIdentifier()));
		}

		template <typename Context>
		static bool IsAlive(Context&& context, state::CardRef target)
		{
			state::Cards::Card const& card = context.state_.mgr.Get(target);
			if (card.GetZone() != state::kCardZonePlay) return false;
			if (card.GetHP() <= 0) return false;
			return true;
		}
	};
}