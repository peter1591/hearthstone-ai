#pragma once

#include "Cards/AuraHelper.h"
#include "Cards/EventRegister.h"
#include "Cards/BattlecryHelper.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	template <typename T,
		typename SpecifiedCardAttributes1 = NoAttribute,
		typename SpecifiedCardAttributes2 = NoAttribute,
		typename SpecifiedCardAttributes3 = NoAttribute,
		typename SpecifiedCardAttributes4 = NoAttribute,
		typename SpecifiedCardAttributes5 = NoAttribute>
	class GeneralCardBase: public state::Cards::CardData
	{
	public:
		void Init(Database::CardData const& data) {
			this->card_id = (Cards::CardId)data.card_id;

			SpecifiedCardAttributes1::Apply(*this);
			SpecifiedCardAttributes2::Apply(*this);
			SpecifiedCardAttributes3::Apply(*this);
			SpecifiedCardAttributes4::Apply(*this);
			SpecifiedCardAttributes5::Apply(*this);

			this->card_type = data.card_type;
			this->card_race = data.card_race;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
			this->enchanted_states.attack = data.attack;
			this->enchanted_states.max_hp = data.max_hp;
		}

		template <typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
		auto Aura() { return AuraHelper<T, EnchantmentType, EmitPolicy, UpdatePolicy>(*this); }
		template <typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
		auto SingleEnchantmentAura() { return SingleEnchantmentAuraHelper<T, EnchantmentType, EmitPolicy, UpdatePolicy>(*this); }
		template <typename EmitPolicy>
		auto BoardFlagAura() { return BoardFlagAuraHelper<T, EmitPolicy, FlowControl::aura::kUpdateOnlyFirstTime>(*this); }
		template <typename EmitPolicy>
		auto OwnerPlayerFlagAura() { return  OwnerPlayerFlagAuraHelper<T, EmitPolicy, FlowControl::aura::kUpdateOwnerChanges>(*this); }

		template <typename Types>
		auto Enrage() { return EnrageHelper<T, Types>(*this); }

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
		static state::CardRef SummonToRight(Context && context, Cards::CardId card_id)
		{
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			int pos = card.GetZonePosition() + 1;
			return SummonInternal(context.manipulate_, card_id, card.GetPlayerIdentifier(), pos);
		}
		static state::CardRef SummonToRight(FlowControl::Manipulate const& manipulate, state::CardRef card_ref, Cards::CardId card_id)
		{
			state::Cards::Card const& card = manipulate.GetCard(card_ref);
			int pos = card.GetZonePosition() + 1;
			return SummonInternal(manipulate, card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static state::CardRef SummonToLeft(Context && context, Cards::CardId card_id)
		{
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			int pos = card.GetZonePosition();
			return SummonInternal(context.manipulate_, card_id, card.GetPlayerIdentifier(), pos);
		}
		static state::CardRef SummonToLeft(FlowControl::Manipulate const& manipulate, state::CardRef card_ref, Cards::CardId card_id)
		{
			state::Cards::Card const& card = manipulate.GetCard(card_ref);
			int pos = card.GetZonePosition();
			return SummonInternal(manipulate, card_id, card.GetPlayerIdentifier(), pos);
		}

		template <typename Context>
		static state::CardRef SummonAt(Context&& context, state::PlayerIdentifier player, int pos, Cards::CardId card_id) {
			int total_minions = (int)context.manipulate_.Board().Player(player).minions_.Size();
			if (pos > total_minions) pos = total_minions;
			return SummonInternal(context.manipulate_, card_id, player, pos);
		}

		static state::CardRef SummonToRightmost(FlowControl::Manipulate const& manipulate, state::PlayerIdentifier player, Cards::CardId card_id)
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

		static std::pair<int, int> GetRandomTwoNumbers(FlowControl::Manipulate const& manipulate, int size) {
			assert(size >= 2);
			int v1 = manipulate.GetRandom().Get(size);
			int v2 = manipulate.GetRandom().Get(size - 1);
			if (v2 >= v1) ++v2;
			return std::make_pair(v1, v2);
		}

		static void DiscardOneRandomHandCard(FlowControl::Manipulate const& manipulate, state::PlayerIdentifier player) {
			auto & hand = manipulate.Board().Player(player).hand_;
			size_t hand_cards = hand.Size();
			if (hand_cards < 1) return;

			size_t rand = manipulate.GetRandom().Get(hand_cards);
			manipulate.Player(player).DiscardHandCard(hand.Get(rand));
		}

		template <typename Functor>
		static void ApplyToAdjacent(FlowControl::Manipulate const& manipulate, state::CardRef card_ref, Functor&& functor) {
			state::PlayerIdentifier player = manipulate.GetCard(card_ref).GetPlayerIdentifier();
			assert(manipulate.GetCard(card_ref).GetZone() == state::kCardZonePlay);

			int pos = manipulate.GetCard(card_ref).GetZonePosition();
			if (pos > 0) {
				functor(manipulate.Board().Player(player).minions_.Get(pos - 1));
			}
			if (pos < (manipulate.Board().Player(player).minions_.Size() - 1)) {
				functor(manipulate.Board().Player(player).minions_.Get(pos + 1));
			}
		}

	public:
		static Cards::CardId GetRandomCardFromDatabase(FlowControl::Manipulate const& manipulate, Cards::Database::CachedCardsTypes type) {
			std::vector<int> const& container = Cards::Database::GetInstance().GetCachedCards(type);
			if (container.empty()) return (Cards::CardId) - 1;
			size_t idx = manipulate.GetRandom().Get(container.size());
			return (Cards::CardId)container[idx];
		}

	private:
		static state::CardRef SummonInternal(FlowControl::Manipulate const& manipulate, Cards::CardId card_id, state::PlayerIdentifier player, int pos)
		{
			if (manipulate.Board().Player(player).minions_.Full()) return state::CardRef();

			return manipulate.Board().SummonMinionById(card_id, player, pos);
		}

		static void SummonInternalByCopy(FlowControl::Manipulate const& manipulate, state::Cards::Card const& card, state::PlayerIdentifier player, int pos)
		{
			if (manipulate.Board().Player(player).minions_.Full()) return;

			manipulate.Board()
				.SummonMinionByCopy(card, player, pos);
		}
	};
}