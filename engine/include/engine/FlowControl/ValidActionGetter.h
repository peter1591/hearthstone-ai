#pragma once

#include <vector>
#include "state/State.h"

namespace engine {
	namespace FlowControl
	{
		class ValidActionGetter
		{
		public:
			ValidActionGetter(state::State const& state) : state_(state) {}

		public:
			state::CardRef GetHeroRef(state::PlayerSide side) const {
				return state_.GetBoard().Get(side).GetHeroRef();
			}

			template <class Functor>
			void ForEachMinion(state::PlayerSide side, Functor&& functor) const {
				state_.GetBoard().Get(side).minions_.ForEach(
					std::forward<Functor>(functor));
			}

		public: // check valid actions
				// These functions MUST return valid for all actually available actions
				// These functions can return valid for ones actually are not valid actions

			template <class Functor>
			void ForEachPlayableCard(Functor && op) const
			{
				auto const& hand = state_.GetCurrentPlayer().hand_;
				for (size_t idx = 0; idx < hand.Size(); ++idx) {
					if (!IsPlayable(idx)) continue;
					if (!op(idx)) return;
				}
			}

			bool IsPlayable(size_t hand_idx) const
			{
				auto const& hand = state_.GetCurrentPlayer().hand_;
				state::CardRef card_ref = hand.Get(hand_idx);
				auto const& card = state_.GetCard(card_ref);

				if (card.GetCardType() == state::kCardTypeMinion) {
					if (state_.GetCurrentPlayer().minions_.Full()) return false;
				}

				if (card.IsSecretCard()) {
					if (state_.GetCurrentPlayer().secrets_.Exists(card.GetCardId())) return false;
				}

				if (!card.GetRawData().onplay_handler.CheckPlayable(state_, state_.GetCurrentPlayerId(), card_ref)) {
					return false;
				}

				if (!CheckCost(card_ref, card)) return false;

				return true;
			}

			bool CanUseHeroPower() const {
				state::CardRef card_ref = state_.GetCurrentPlayer().GetHeroPowerRef();
				state::Cards::Card const& card = state_.GetCard(card_ref);

				bool ret = false;
				if (card.GetRawData().usable) {
					ret = CheckCost(card_ref, card);
				}

				return ret;
			}

		public:
			bool IsAttackable(state::CardRef card_ref) const {
				return IsAttackable(state_.GetCardsManager().Get(card_ref));
			}

			bool IsAttackable(state::Cards::Card const& card) const {
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

			// TODO: why encode attacker? just return state::CardRef
			// return list of encoded indices
			// encoded index:
			//   0 ~ 6: minion index from left to right
			//   7: hero
			template <typename Functor>
			void ForEachAttacker(Functor&& functor) const {
				auto const& player = state_.GetBoard().Get(state_.GetCurrentPlayerId());
				auto op = [&](state::CardRef card_ref) { return IsAttackable(card_ref); };

				int minion_idx = 0;
				if (!player.minions_.ForEach([&](state::CardRef card_ref) {
					if (op(card_ref)) {
						if (!functor(minion_idx)) return false;
					}
					++minion_idx;
					return true;
				})) return;

				state::CardRef hero_ref = player.GetHeroRef();
				if (op(hero_ref)) {
					if (!functor(7)) return;
				}
			}

			// Decode attacker index. See @ForEachAttacker
			state::CardRef GetFromAttackerIndex(int attacker_idx) const {
				if (attacker_idx == 7) {
					return state_.GetCurrentPlayer().GetHeroRef();
				}

				assert(attacker_idx >= 0);
				assert(attacker_idx < 7);
				auto const& minions = state_.GetCurrentPlayer().minions_;
				if (attacker_idx >= (int)minions.Size()) return state::CardRef(); // invalid card ref
				return minions.Get((size_t)(attacker_idx));
			}

			std::array<state::CardRef, 8> GetAttackerIndics() const {
				std::array<state::CardRef, 8> ret;
				for (int i = 0; i < 8; ++i) {
					ret[i] = GetFromAttackerIndex(i);
				}
				return ret;
			}

			std::vector<state::CardRef> GetDefenders() const {
				std::vector<state::CardRef> defenders;
				auto const& player = state_.GetBoard().Get(state_.GetCurrentPlayerId().Opposite());

				state::CardRef hero_ref = player.GetHeroRef();
				defenders.push_back(hero_ref);

				player.minions_.ForEach([&](state::CardRef card_ref) {
					defenders.push_back(card_ref);
					return true;
				});

				return defenders;
			}

			bool HeroPowerUsable() const {
				state::CardRef hero_power_ref = state_.GetCurrentPlayer().GetHeroPowerRef();
				if (!hero_power_ref.IsValid()) return false;
				if (!state_.GetCard(hero_power_ref).GetRawData().usable) return false;

				// TODO: need to consider the cost_health_instead flag
				int resource = state_.GetCurrentPlayer().GetResource().GetCurrent();
				if (state_.GetCard(hero_power_ref).GetCost() > resource) return false;

				return true;
			}

		private:
			bool CheckCost(state::CardRef card_ref, state::Cards::Card const& card) const
			{
				int cost = card.GetCost();
				bool cost_health_instead = false;

				state_.TriggerEventWithoutRemove<state::Events::EventTypes::GetPlayCardCost>(state::Events::EventTypes::GetPlayCardCost::Context{
					state_, card_ref, &cost, &cost_health_instead
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
}