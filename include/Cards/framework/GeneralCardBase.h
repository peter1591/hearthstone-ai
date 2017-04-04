#pragma once

#include "Cards/AuraHelper.h"
#include "Cards/EventRegister.h"
#include "Cards/BattlecryHelper.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	template <typename T>
	class GeneralCardBase: public state::Cards::CardData
	{
	public:
		template <typename... Types>
		auto Aura() { return AuraHelper<T, Types...>(*this); }

		template <typename... Types>
		auto Enrage() { return EnrageHelper<T, Types...>(*this); }

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler = T>
		using RegisteredEventType = OneEventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler>;

		template <typename EventType, typename EventHandler = T>
		using RegisteredManagedEventType = ManagedOneEventRegisterHelper<EventType, EventHandler>;

		template <typename... RegisteredEvents>
		auto RegisterEvents() {
			return EventsRegisterHelper<RegisteredEvents...>::Process((state::Cards::CardData&)*this);
		}

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler = T>
		auto RegisterEvent() {
			return RegisterEvents<RegisteredEventType<LifeTime, SelfPolicy, EventType, EventHandler>>();
		}

		template <typename EventType, typename EventHandler = T>
		auto RegisterEvent() {
			return RegisterEvents<RegisteredManagedEventType<EventType, EventHandler>>();
		}


		template <typename Context>
		static void SummonToRight(Context && context, Cards::CardId card_id)
		{
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			int pos = card.GetZonePosition() + 1;
			return SummonInternal(std::forward<Context>(context), card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonToLeft(Context && context, Cards::CardId card_id)
		{
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			int pos = card.GetZonePosition();
			return SummonInternal(std::forward<Context>(context), card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonAt(Context&& context, state::PlayerIdentifier player, int pos, Cards::CardId card_id) {
			int total_minions = (int)context.manipulate_.Board().Player(player).minions_.Size();
			if (pos > total_minions) pos = total_minions;
			return SummonInternal(std::forward<Context>(context), card_id, player, pos);
		}

		template <typename Context>
		static void SummonToEnemy(Context && context, Cards::CardId card_id)
		{
			state::PlayerIdentifier player = context.card_.GetPlayerIdentifier().Opposite();
			int pos = (int)context.manipulate_.Board().Player(player).minions_.Size();
			return SummonInternal(std::forward<Context>(context), card_id, player, pos);
		}

		template <typename Context>
		static void SummonToPlayerByCopy(Context context, state::PlayerIdentifier player, state::Cards::Card const& card)
		{
			int pos = (int)context.manipulate_.Board().Player(player).minions_.Size();
			return SummonInternalByCopy(std::move(context), card, player, pos);
		}

	private:
		template <typename Context>
		static void SummonInternal(Context&& context, Cards::CardId card_id, state::PlayerIdentifier player, int pos)
		{
			if (context.manipulate_.Board().Player(player).minions_.Full()) return;

			context.manipulate_.Board()
				.SummonMinionById(card_id, player, pos);
		}

		template <typename Context>
		static void SummonInternalByCopy(Context context, state::Cards::Card const& card, state::PlayerIdentifier player, int pos)
		{
			if (context.manipulate_.Board().Player(player).minions_.Full()) return;

			context.manipulate_.Board()
				.SummonMinionByCopy(card, player, pos);
		}
	};
}