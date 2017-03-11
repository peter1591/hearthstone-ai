#pragma once

#include "FlowControl/Contexts.h"
#include "state/State.h"
#include "Cards/TargetorUtils.h"

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

		bool IsMinionsFull() const
		{
			return player_.minions_.Full();
		}

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
				target_ = context_.state_.board.Get(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}
			DamageHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.state_.board.Get(context_.card_.GetPlayerIdentifier().Opposite()).hero_ref_;
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
				target_ = context_.state_.board.Get(context_.card_.GetPlayerIdentifier()).hero_ref_;
				return *this;
			}
			HealHelper & Opponent() {
				assert(!target_.IsValid());
				target_ = context_.state_.board.GetAnother(context_.card_.GetPlayerIdentifier()).hero_ref_;
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

		static TargetorHelper Targets() { return TargetorHelper(); }

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
			return PlayerHelper<Context>(context, context.state_.board.Get(context.card_.GetPlayerIdentifier().Opposite()));
		}

		template <typename Context>
		static bool IsAlive(Context&& context, state::CardRef target)
		{
			state::Cards::Card const& card = context.state_.mgr.Get(target);
			return IsAlive(context, card);
		}

		template <typename Context>
		static bool IsAlive(Context&& context, state::Cards::Card const& target)
		{
			if (!IsInPlay(context, target)) return false;
			if (IsMortallyWounded(context, target)) return false;
			// TODO: pending destroy
			return true;
		}

		template <typename Context>
		static bool IsMortallyWounded(Context&& context, state::Cards::Card const& target)
		{
			if (target.GetHP() <= 0) return true;
			return false;
		}

		template <typename Context>
		static bool IsInPlay(Context&& context, state::Cards::Card const& target)
		{
			if (target.GetZone() == state::kCardZonePlay) return true;
			return false;
		}

		static void SetBattlecryTarget(
			FlowControl::Context::BattlecryTargetGetter & context,
			TargetorHelper const& targetor)
		{
			context.flow_context_.battlecry_target_ =
				context.flow_context_.action_parameters_.GetBattlecryTarget(
					context.state_, context.card_ref_, context.card_,
					targetor.GetInfo());
		}

		template <typename Context>
		static void Summon(Context && context, int card_id)
		{
			if (Player(context).IsMinionsFull()) return;

			state::Cards::CardData card_data = FlowControl::Dispatchers::Minions::CreateInstance(card_id);

			card_data.enchantable_states.player = context.card_.GetPlayerIdentifier();
			card_data.zone = state::kCardZonePlay;
			card_data.zone_position = context.card_.GetZonePosition() + 1;

			assert(card_data.zone_position <= 6);

			card_data.enchantment_aux_data.origin_states = card_data.enchantable_states;

			state::CardRef ref = context.state_.mgr.PushBack(
				context.state_, context.flow_context_, state::Cards::Card(card_data));

			FlowControl::Manipulate(context.state_, context.flow_context_)
				.Board().Summon(ref);
		}

		template <typename Context>
		static void ForEach(
			Context & context,
			TargetorHelper const& targetor,
			TargetorInfo::FuncForEach *func)
		{
			targetor.GetInfo().ForEach(context.state_, context.flow_context_, func);
		}
	};
}