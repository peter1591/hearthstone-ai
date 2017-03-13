#pragma once

#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"
#include "detail/ZonePositionSetter.h"

namespace FlowControl
{
	class FlowContext;
}

namespace state
{
	class State
	{
	public:
		board::Board const& GetBoard() const { return board_; }
		board::Board & GetBoard() { return board_; }

		Cards::Manager const& GetCardsManager() const { return cards_mgr_; }

		Events::Manager const& GetEventsManager() const { return event_mgr_; }
		Events::Manager & GetEventsManager() { return event_mgr_; }

		PlayerIdentifier const& GetCurrentPlayerId() const { return current_player_; }
		PlayerIdentifier & GetMutableCurrentPlayerId() { return current_player_; }
		board::Player & GetCurrentPlayer() { return board_.Get(current_player_); }
		const board::Player & GetCurrentPlayer() const { return board_.Get(current_player_); }
		board::Player & GetOppositePlayer() { return board_.Get(current_player_.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board_.Get(current_player_.Opposite()); }

		int GetTurn() const { return turn_; }
		void SetTurn(int turn) { turn = turn_; }
		void IncreaseTurn() { ++turn_; }

	public: // cards manager
		Cards::Card const& GetCard(CardRef ref) const { return cards_mgr_.Get(ref); }
		CardRef AddCard(Cards::Card&& card) { return cards_mgr_.PushBack(std::move(card)); }

		FlowControl::Manipulators::CardManipulator GetCardManipulator(State& state, FlowContext& flow_context, CardRef ref) {
			return FlowControl::Manipulators::CardManipulator(state, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

		FlowControl::Manipulators::HeroManipulator GetHeroManipulator(State& state, FlowContext& flow_context, CardRef ref) {
			return FlowControl::Manipulators::HeroManipulator(state, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

		FlowControl::Manipulators::MinionManipulator GetMinionManipulator(State& state, FlowContext& flow_context, CardRef ref) {
			return FlowControl::Manipulators::MinionManipulator(state, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

		FlowControl::Manipulators::CharacterManipulator GetCharacterManipulator(State& state, FlowContext& flow_context, CardRef ref) {
			return FlowControl::Manipulators::CharacterManipulator(state, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

		FlowControl::Manipulators::WeaponManipulator GetWeaponManipulator(State& state, FlowContext& flow_context, CardRef ref) {
			return FlowControl::Manipulators::WeaponManipulator(state, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

	public: // mutate functions
		// push new card --> card_ref
		// change card_ref zone
		// trigger events
		// set card cost/attack/hp/etc.

		ZoneChangerWithUnknownZoneUnknownType GetZoneChanger(FlowContext& flow_context, CardRef ref)
		{
			return ZoneChangerWithUnknownZoneUnknownType(board_, cards_mgr_, flow_context, ref, cards_mgr_.GetMutable(ref));
		}

		// TODO: should not be public
		void SetCardZonePos(CardRef ref, int pos)
		{
			cards_mgr_.SetCardZonePos(ref, pos);
		}

	private:
		board::Board board_;
		Cards::Manager cards_mgr_;
		Events::Manager event_mgr_;

		PlayerIdentifier current_player_;
		int turn_;
	};
}
