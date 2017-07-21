#pragma once

#include <vector>
#include "state/State.h"

namespace FlowControl
{
	class ValidActionGetter
	{
	public:
		ValidActionGetter(state::State const& state) : state_(state) {}

	public: // check valid actions
			// These functions MUST return valid for all actually available actions
			// These functions can return valid for ones actually are not valid actions

		template <class Functor>
		void ForEachPlayableCard(Functor && op)
		{
			auto const& hand = state_.GetCurrentPlayer().hand_;
			for (size_t idx = 0; idx < hand.Size(); ++idx) {
#ifndef NDEBUG
				// TODO: added it back after implemented the comparison operator
				// state::State origin_state = state_;
#endif

				if (!IsPlayable(idx)) continue;
				if (!op(idx)) return;

#ifndef NDEBUG
				// TODO: implement comparison operator
				//assert(origin_state == state_);
#endif
			}
		}

		bool IsPlayable(size_t hand_idx)
		{
			// we use mutable state here to invoke the event
			// but the event handlers should NOT modify state
			// 'origin_state' helps to detect logic error in debug builds
			// TODO: re-write GetPlayCardCost() event handlers to get rid of this
			state::State & mutable_state = const_cast<state::State &>(state_);

			auto const& hand = state_.GetCurrentPlayer().hand_;
			state::CardRef card_ref = hand.Get(hand_idx);
			auto const& card = state_.GetCard(card_ref);

			if (card.GetCardType() == state::kCardTypeMinion) {
				if (state_.GetCurrentPlayer().minions_.Full()) return false;
			}

			if (card.IsSecretCard()) {
				if (state_.GetCurrentPlayer().secrets_.Exists(card.GetCardId())) return false;
			}

			if (!CheckCost(card_ref, card, mutable_state)) {
				return false;
			}

			return true;
		}

		bool CanUseHeroPower() {
#ifndef NDEBUG
			// TODO: added it back after implemented the comparison operator
			// state::State origin_state = state_;
#endif
			// we use mutable state here to invoke the event
			// but the event handlers should NOT modify state
			// 'origin_state' helps to detect logic error in debug builds
			// TODO: re-write GetPlayCardCost() event handlers to get rid of this
			state::State & mutable_state = const_cast<state::State &>(state_);

			state::CardRef card_ref = state_.GetCurrentPlayer().GetHeroPowerRef();
			state::Cards::Card const& card = state_.GetCard(card_ref);

			bool ret = false;
			if (card.GetRawData().usable) {
				ret = CheckCost(card_ref, card, mutable_state);
			}

#ifndef NDEBUG
			// TODO: implement comparison operator
			//assert(origin_state == state_);
#endif
			return ret;
		}

	public:
		bool IsAttackable(state::CardRef card_ref) {
			return IsAttackable(state_.GetCardsManager().Get(card_ref));
		}

		bool IsAttackable(state::Cards::Card const& card) {
			assert(card.GetHP() > 0);
			assert(card.GetZone() == state::kCardZonePlay);

			if (card.GetRawData().cant_attack) return false;
			if (card.GetRawData().freezed) return false;

			if (card.GetCardType() == state::kCardTypeMinion) {
				if (card.HasCharge() == false && card.GetRawData().just_played) return false;
			}

			if (card.GetRawData().num_attacks_this_turn >= card.GetMaxAttacksPerTurn()) return false;

			if (state_.GetCardAttackConsiderWeapon(card) <= 0) return false;

			return true;
		}

		// return list of encoded indices
		// encoded index:
		//   0 ~ 6: minion index from left to right
		//   7: hero
		std::vector<int> GetAttackers() {
			std::vector<int> attackers;
			auto const& player = state_.GetBoard().Get(state_.GetCurrentPlayerId());
			auto op = [&](state::CardRef card_ref) { return IsAttackable(card_ref); };

			int minion_idx = 0;
			player.minions_.ForEach([&](state::CardRef card_ref) {
				if (op(card_ref)) attackers.push_back(minion_idx);
				++minion_idx;
			});

			state::CardRef hero_ref = player.GetHeroRef();
			if (op(hero_ref)) attackers.push_back(7);
			
			return attackers;
		}

		std::vector<state::CardRef> GetDefenders() {
			std::vector<state::CardRef> defenders;
			auto const& player = state_.GetBoard().Get(state_.GetCurrentPlayerId().Opposite());

			state::CardRef hero_ref = player.GetHeroRef();
			defenders.push_back(hero_ref);

			player.minions_.ForEach([&](state::CardRef card_ref) {
				defenders.push_back(card_ref);
			});

			return defenders;
		}

		bool HeroPowerUsable() {
			state::CardRef hero_power_ref = state_.GetCurrentPlayer().GetHeroPowerRef();
			if (!hero_power_ref.IsValid()) return false;
			if (!state_.GetCard(hero_power_ref).GetRawData().usable) return false;

			// TODO: need to consider the cost_health_instead flag
			int resource = state_.GetCurrentPlayer().GetResource().GetCurrent();
			if (state_.GetCard(hero_power_ref).GetCost() > resource) return false;
			
			return true;
		}

	private:
		bool CheckCost(state::CardRef card_ref, state::Cards::Card const& card, state::State & mutable_state)
		{
			int cost = card.GetCost();
			bool cost_health_instead = false;

			FlowContext & fake_flow_context = *((FlowContext *)nullptr);
			mutable_state.TriggerEvent<state::Events::EventTypes::GetPlayCardCost>(state::Events::EventTypes::GetPlayCardCost::Context{
				Manipulate(mutable_state, fake_flow_context), card_ref, &cost, &cost_health_instead
			});

			if (cost <= 0) return true;

			if (cost_health_instead) {
				state::CardRef hero_ref = state_.GetCurrentPlayer().GetHeroRef();
				if (state_.GetCard(hero_ref).GetHP() < cost) return false;
			}
			else {
				auto& crystal = state_.GetCurrentPlayer().GetResource();
				if (crystal.GetCurrent() < cost) return false;
			}

			return true;
		}
		
	private:
		state::State const& state_;
	};
}