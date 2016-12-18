#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroManipulator::DrawCard()
		{
			state::board::Player & player = state_.board.Get(player_id_);
			assert(player.hero_ref_ == card_ref_);

			if (player.deck_.Empty())
			{
				int damage = ++player.fatigue_damage_;
				this->Damage(damage);
				return;
			}

			int deck_count = (int)player.deck_.Size();
			int deck_idx = 0;
			if (deck_count > 1) deck_idx = flow_context_.random_.Get(deck_count);

			state::CardRef card_ref = player.deck_.Get(deck_idx);

			if (player.hand_.Full()) {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().ChangeTo<state::kCardZoneGraveyard>(player_id_);
			}
			else {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().ChangeTo<state::kCardZoneHand>(player_id_);
			}

			// TODO: trigger on-draw event (parameter: card_ref)
		}
	}
}