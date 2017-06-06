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

	private:
		state::State const& state_;
	};
}