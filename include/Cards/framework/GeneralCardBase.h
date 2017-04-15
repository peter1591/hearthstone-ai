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
		template <typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
		auto Aura() { return AuraHelper<T, EnchantmentType, EmitPolicy, UpdatePolicy>(*this); }
		template <typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
		auto SingleEnchantmentAura() { return SingleEnchantmentAuraHelper<T, EnchantmentType, EmitPolicy, UpdatePolicy>(*this); }
		template <typename EmitPolicy>
		auto BoardFlagAura() { return BoardFlagAuraHelper<T, EmitPolicy, FlowControl::aura::kUpdateOnlyFirstTime>(*this); }
		template <typename EmitPolicy>
		auto OwnerPlayerFlagAura() { return  OwnerPlayerFlagAuraHelper<T, EmitPolicy, FlowControl::aura::kUpdateOwnerChanges>(*this); }

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
			return SummonInternal(context.manipulate_, card_id, card.GetPlayerIdentifier(), pos);
		}
		static void SummonToRight(FlowControl::Manipulate & manipulate, state::CardRef card_ref, Cards::CardId card_id)
		{
			state::Cards::Card const& card = manipulate.GetCard(card_ref);
			int pos = card.GetZonePosition() + 1;
			return SummonInternal(manipulate, card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonToLeft(Context && context, Cards::CardId card_id)
		{
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			int pos = card.GetZonePosition();
			return SummonInternal(context.manipulate_, card_id, card.GetPlayerIdentifier(), pos);
		}
		static void SummonToLeft(FlowControl::Manipulate & manipulate, state::CardRef card_ref, Cards::CardId card_id)
		{
			state::Cards::Card const& card = manipulate.GetCard(card_ref);
			int pos = card.GetZonePosition();
			return SummonInternal(manipulate, card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static void SummonAt(Context&& context, state::PlayerIdentifier player, int pos, Cards::CardId card_id) {
			int total_minions = (int)context.manipulate_.Board().Player(player).minions_.Size();
			if (pos > total_minions) pos = total_minions;
			return SummonInternal(context.manipulate_, card_id, player, pos);
		}

		static void SummonToRightmost(FlowControl::Manipulate & manipulate, state::PlayerIdentifier player, Cards::CardId card_id)
		{
			int pos = (int)manipulate.Board().Player(player).minions_.Size();
			return SummonInternal(manipulate, card_id, player, pos);
		}

		template <typename Context>
		static void SummonToPlayerByCopy(Context context, state::PlayerIdentifier player, state::Cards::Card const& card)
		{
			int pos = (int)context.manipulate_.Board().Player(player).minions_.Size();
			return SummonInternalByCopy(context.manipulate_, card, player, pos);
		}

	private:
		static void SummonInternal(FlowControl::Manipulate & manipulate, Cards::CardId card_id, state::PlayerIdentifier player, int pos)
		{
			if (manipulate.Board().Player(player).minions_.Full()) return;

			manipulate.Board()
				.SummonMinionById(card_id, player, pos);
		}

		static void SummonInternalByCopy(FlowControl::Manipulate & manipulate, state::Cards::Card const& card, state::PlayerIdentifier player, int pos)
		{
			if (manipulate.Board().Player(player).minions_.Full()) return;

			manipulate.Board()
				.SummonMinionByCopy(card, player, pos);
		}
	};
}