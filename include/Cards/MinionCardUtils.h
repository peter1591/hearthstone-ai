#pragma once

#include "FlowControl/Contexts.h"
#include "state/State.h"
#include "state/targetor/TargetsGenerator.h"
#include "Cards/id-map.h"
#include "Cards/Contexts.h"

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
			player_.GetResource().IncreaseTotal(amount);
		}

		void GainCrystal(int amount)
		{
			player_.GetResource().IncreaseTotal(amount);
			player_.GetResource().IncreaseCurrent(amount);
		}

		bool IsMinionsFull() const { return player_.minions_.Full(); }
		auto GetMinionsCount() const { return player_.minions_.Size(); }

	private:
		Context & context_;
		state::board::Player & player_;
	};

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
				target_ = context_.state_.GetBoard().Get(context_.card_.GetPlayerIdentifier()).GetHeroRef();
				return *this;
			}
			DamageHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.state_.GetBoard().Get(context_.card_.GetPlayerIdentifier().Opposite()).GetHeroRef();
				return *this;
			}

			void Amount(int amount) {
				assert(target_.IsValid());
				FlowControl::Manipulate(context_.state_, context_.flow_context_).Character(target_).Damage(amount);
			}

		private:
			state::CardRef target_;
			Context & context_;
		};

		template <typename Context>
		class HealHelper
		{
		public:
			template <typename T>
			HealHelper(T&& context) : context_(std::forward<T>(context)) {}

			HealHelper & Target(state::CardRef target) {
				assert(!target_.IsValid());
				target_ = target;
				return *this;
			}
			HealHelper & Owner() {
				assert(!target_.IsValid());
				target_ = context_.state_.GetBoard().Get(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}
			HealHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.state_.GetBoard().GetAnother(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}

			void Amount(int amount) {
				assert(target_.IsValid());
				FlowControl::Manipulate(context_.state_, context_.flow_context_)
					.Character(target_).Heal(amount);
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
		static HealHelper<std::decay_t<Context>> Heal(Context&& context)
		{
			return HealHelper<std::decay_t<Context>>(std::forward<Context>(context));
		}

		static state::targetor::TargetsGenerator Targets() { return state::targetor::TargetsGenerator(); }

		template <typename Context>
		static state::PlayerIdentifier OwnedPlayer(Context&& context)
		{
			return context.card_.GetPlayerIdentifier();
		}

		template <typename Context>
		static state::CardRef Owner(Context&& context)
		{
			return context.state_.GetBoard().Get(context.card_.GetPlayerIdentifier()).hero_ref_;
		}

		template <typename Context>
		static state::CardRef Opponent(Context&& context)
		{
			return context.state_.GetBoard().GetAnother(context.card_.GetPlayerIdentifier()).hero_ref_;
		}

		template <typename Context>
		static PlayerHelper<Context> Player(Context&& context, state::PlayerIdentifier player)
		{
			return PlayerHelper<Context>(context, context.state_.GetBoard().Get(player));
		}

		template <typename Context>
		static PlayerHelper<Context> Player(Context&& context)
		{
			return PlayerHelper<Context>(context, context.state_.GetBoard().Get(context.card_.GetPlayerIdentifier()));
		}

		template <typename Context>
		static PlayerHelper<Context> AnotherPlayer(Context&& context)
		{
			return PlayerHelper<Context>(context, context.state_.GetBoard().Get(context.card_.GetPlayerIdentifier().Opposite()));
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

		template <typename Context>
		static void SummonToRight(Context && context, int card_id)
		{
			int pos = context.card_.GetZonePosition() + 1;
			return SummonInternal(std::forward<Context>(context), card_id, context.card_.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonToLeft(Context && context, int card_id)
		{
			int pos = context.card_.GetZonePosition();
			return SummonInternal(std::forward<Context>(context), card_id, context.card_.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonToEnemy(Context && context, int card_id)
		{
			state::PlayerIdentifier player = context.card_.GetPlayerIdentifier().Opposite();
			int pos = context.state_.GetBoard().Get(player.Opposite()).minions_.Size();
			return SummonInternal(std::forward<Context>(context), card_id, player, pos);
		}

	private:
		template <typename Context>
		static void SummonInternal(Context&& context, int card_id, state::PlayerIdentifier player, int pos)
		{
			if (context.state_.GetBoard().Get(player).minions_.Full()) return;

			state::Cards::CardData card_data = FlowControl::Dispatchers::Minions::CreateInstance(card_id);
			card_data.enchantable_states.player = player;

			FlowControl::Manipulate(context.state_, context.flow_context_)
				.Board().Summon(std::move(card_data), player, pos);
		}

	public:
		template <typename Context, typename Functor>
		static void ForEach(
			Context&& context,
			state::targetor::TargetsGenerator_Leaf const& targets_generator,
			Functor&& func)
		{
			targets_generator.GetInfo().ForEach(context.state_, context.flow_context_, func);
		}
	};
}