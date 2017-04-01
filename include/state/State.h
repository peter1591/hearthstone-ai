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
		State() : turn_(0), play_order_(1) {}

		board::Board const& GetBoard() const { return board_; }
		board::Board & GetBoard() { return board_; }

		Cards::Manager const& GetCardsManager() const { return cards_mgr_; }

		aura::Manager const& GetAuraManager() const { return aura_mgr_; }
		aura::Manager & GetAuraManager() { return aura_mgr_; }

		aura::Manager const& GetFlagAuraManager() const { return flag_aura_mgr_; }
		aura::Manager & GetFlagAuraManager() { return flag_aura_mgr_; }

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

	public: // bridge to event manager
		template <typename EventTriggerType, typename... Args>
		void TriggerEvent(Args&&... args) {
			return event_mgr_.TriggerEvent<EventTriggerType, Args...>(std::forward<Args>(args)...);
		}
		template <typename EventTriggerType, typename... Args>
		void TriggerCategorizedEvent(CardRef card_ref, Args&&... args) {
			return event_mgr_.TriggerCategorizedEvent<EventTriggerType, Args...>(card_ref, std::forward<Args>(args)...);
		}

	public: // zone changer
		ZoneChangerWithUnknownZoneUnknownType GetZoneChanger(FlowControl::Manipulate & manipulate, CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownZoneUnknownType>(manipulate, ref);
		}

		template <state::CardZone KnownZone>
		ZoneChangerWithUnknownType<KnownZone> GetZoneChanger(FlowControl::Manipulate & manipulate,  CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownType<KnownZone>>(manipulate, ref);
		}

		template <state::CardType KnownType>
		ZoneChangerWithUnknownZone<KnownType> GetZoneChanger(FlowControl::Manipulate & manipulate,  CardRef ref) {
			return GetZoneChanger<ZoneChangerWithUnknownZone<KnownType>>(manipulate, ref);
		}

		template <state::CardType KnownType, state::CardZone KnownZone>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(FlowControl::Manipulate & manipulate,  CardRef ref) {
			return GetZoneChanger<ZoneChanger<KnownZone, KnownType>>(manipulate, ref);
		}

		template <state::CardZone KnownZone, state::CardType KnownType>
		ZoneChanger<KnownZone, KnownType> GetZoneChanger(FlowControl::Manipulate & manipulate,  CardRef ref) {
			return GetZoneChanger<ZoneChanger<KnownZone, KnownType>>(manipulate, ref);
		}

	private:
		template <typename ZoneChangerType>
		ZoneChangerType GetZoneChanger(FlowControl::Manipulate & manipulate, CardRef ref) {
			return ZoneChangerType(*this, board_, cards_mgr_, manipulate, ref);
		}

	private:
		board::Board board_;
		Cards::Manager cards_mgr_;
		Events::Manager event_mgr_;
		aura::Manager aura_mgr_;
		aura::Manager flag_aura_mgr_;

		PlayerIdentifier current_player_;
		int turn_;
		int play_order_;
	};
}
