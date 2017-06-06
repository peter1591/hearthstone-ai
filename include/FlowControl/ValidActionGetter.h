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
		std::vector<int> GetPlayableCards() {
			std::vector<int> playable_cards;
			int resource = state_.GetCurrentPlayer().GetResource().GetCurrent();
			auto const& hand = state_.GetCurrentPlayer().hand_;
			for (int i = 0; i < hand.Size(); ++i) {
				state::CardRef card_ref = hand.Get(i);
				if (state_.GetCard(card_ref).GetCost() > resource) continue;

				playable_cards.push_back(i);
			}
			return playable_cards;
		}

		std::vector<state::CardRef> GetAttackers() {
			std::vector<state::CardRef> attackers;
			auto const& player = state_.GetBoard().Get(state_.GetCurrentPlayerId());

			state::CardRef hero_ref = player.GetHeroRef();
			if (state_.GetCard(hero_ref).GetAttack() > 0) attackers.push_back(hero_ref);

			player.minions_.ForEach([&](state::CardRef card_ref) {
				if (state_.GetCard(card_ref).GetAttack() > 0) attackers.push_back(card_ref);
			});
			
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
		
	private:
		state::State const& state_;
	};
}