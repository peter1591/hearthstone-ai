#pragma once

#include "state/Types.h"
#include "state/aura/Manager.h"
#include "state/board/Board.h"
#include "state/Cards/Manager.h"
#include "state/Events/Manager.h"
#include "state/IRandomGenerator.h"
#include "state/ZoneChanger.h"

// implementation details
#include "state/detail/PlayerDataStructureMaintainer-impl.h"

namespace FlowControl { class Manipulate; }

namespace state
{
	class State
	{
		template <CardZone, CardType> friend class ZoneChanger;

	public:
		State() :
			board_(), cards_mgr_(), event_mgr_(), aura_mgr_(),
			current_player_(), turn_(0), play_order_(1)
		{}

		// ConstructWithBaseState
		State(State const* base) :
			board_(),
			cards_mgr_(&base->cards_mgr_), event_mgr_(), aura_mgr_(),
			current_player_(), turn_(0), play_order_(1)
		{
			board_ = base->board_;
			event_mgr_ = base->event_mgr_;
			aura_mgr_ = base->aura_mgr_;
			current_player_ = base->current_player_;
			turn_ = base->turn_;
			play_order_ = base->play_order_;
		}

	public:
		board::Board const& GetBoard() const { return board_; }
		board::Board & GetBoard() { return board_; }

		Cards::Manager const& GetCardsManager() const { return cards_mgr_; }

		aura::Manager const& GetAuraManager() const { return aura_mgr_; }
		aura::Manager & GetAuraManager() { return aura_mgr_; }

		PlayerIdentifier const& GetCurrentPlayerId() const { return current_player_; }
		PlayerIdentifier & GetMutableCurrentPlayerId() { return current_player_; }
		board::Player & GetCurrentPlayer() { return board_.Get(current_player_); }
		const board::Player & GetCurrentPlayer() const { return board_.Get(current_player_); }
		board::Player & GetOppositePlayer() { return board_.Get(current_player_.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board_.Get(current_player_.Opposite()); }

		int GetTurn() const { return turn_; }
		void SetTurn(int turn) { turn_ = turn; }
		void IncreaseTurn() { ++turn_; }

		int GetPlayOrder() const { return play_order_; }
		void SetPlayOrder(int play_order) { play_order_ = play_order; }
		void IncreasePlayOrder() { ++play_order_; }

	public: // bridge to cards manager
		Cards::Card const& GetCard(CardRef ref) const { return cards_mgr_.Get(ref); }
		Cards::Card & GetMutableCard(CardRef ref) { return cards_mgr_.GetMutable(ref); }
		CardRef AddCard(Cards::Card&& card) { return cards_mgr_.PushBack(std::move(card)); }

		// Accumulator: bool(T& val, state::Cards::Card const& card)
		//    @return: true to continue; false to stop and return val
		template <typename T, typename Accumulator>
		T GetCardAttributeConsiderWeapon(state::Cards::Card const& card, T val, Accumulator accumulator) const
		{
			if (!accumulator(val, card)) return val;

			if (card.GetCardType() != kCardTypeHero) return val;
			state::CardRef weapon_ref = board_.Get(card.GetPlayerIdentifier()).GetWeaponRef();
			if (!weapon_ref.IsValid()) return val;

			auto const& weapon_card = GetCard(weapon_ref);
			accumulator(val, weapon_card);
			return val;
		}
		template <typename Functor>
		bool GetCardBoolAttributeConsiderWeapon(state::CardRef card_ref, Functor && functor) const {
			return GetCardBoolAttributeConsiderWeapon(GetCard(card_ref), std::forward<Functor>(functor));
		}
		template <typename Functor>
		bool GetCardBoolAttributeConsiderWeapon(state::Cards::Card const& card, Functor && functor) const {
			return GetCardAttributeConsiderWeapon(card, false, [&](bool& val, state::Cards::Card const& card) {
				assert(val == false); // if val is true; then it should be short-circuited in the last call
				val = functor(card);
				return !val; // if val is true, then we can short-circuit now
			});
		}

		int GetCardAttackConsiderWeapon(state::CardRef card_ref) const {
			return GetCardAttackConsiderWeapon(GetCard(card_ref));
		}
		int GetCardAttackConsiderWeapon(state::Cards::Card const& card) const {
			return GetCardAttributeConsiderWeapon(card, 0, [](int& val, state::Cards::Card const& card) {
				val += card.GetAttack();
				return true;
			});
		}

	public: // bridge to event manager
		template <typename EventType, typename T>
		void AddEvent(T&& handler) {
			return event_mgr_.PushBack<EventType, T>(std::forward<T>(handler));
		}
		template <typename EventType, typename T>
		void AddEvent(CardRef card_ref, T&& handler) {
			return event_mgr_.PushBack<EventType, T>(card_ref, std::forward<T>(handler));
		}
		template <typename EventTriggerType, typename... Args>
		void TriggerEvent(Args&&... args) {
			return event_mgr_.TriggerEvent<EventTriggerType, Args...>(std::forward<Args>(args)...);
		}
		template <typename EventTriggerType, typename... Args>
		void TriggerCategorizedEvent(CardRef card_ref, Args&&... args) {
			return event_mgr_.TriggerCategorizedEvent<EventTriggerType, Args...>(card_ref, std::forward<Args>(args)...);
		}

	public: // zone changer
		ZoneChangerWithUnknownZoneUnknownType GetZoneChanger(CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownZoneUnknownType>(ref);
		}

		template <state::CardZone KnownZone>
		ZoneChangerWithUnknownType<KnownZone> GetZoneChanger(CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownType<KnownZone>>(ref);
		}

		template <state::CardType KnownType>
		ZoneChangerWithUnknownZone<KnownType> GetZoneChanger(CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownZone<KnownType>>(ref);
		}

		template <state::CardType KnownType, state::CardZone KnownZone>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(CardRef ref) {
			return GetZoneChanger<ZoneChanger<KnownZone, KnownType>>(ref);
		}

		template <state::CardZone KnownZone, state::CardType KnownType>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(CardRef ref) {
			return GetZoneChanger<ZoneChanger<KnownZone, KnownType>>(ref);
		}

	private:
		template <typename ZoneChangerType>
		ZoneChangerType GetZoneChanger(CardRef ref) {
			return ZoneChangerType(*this, board_, cards_mgr_, ref);
		}

	private:
		board::Board board_;
		Cards::Manager cards_mgr_;
		Events::Manager event_mgr_;
		aura::Manager aura_mgr_;

		PlayerIdentifier current_player_;
		int turn_;
		int play_order_;
	};
}
