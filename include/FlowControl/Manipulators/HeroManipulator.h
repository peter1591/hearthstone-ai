#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::PlayerIdentifier player_id)
				: CharacterManipulator(state, flow_context, card_ref, card), player_id_(player_id)
			{
				assert(card.GetCardType() == state::kCardTypeHero);
			}

			template <typename RandomGenerator>
			void DrawCard(RandomGenerator&& random);

		private:
			state::PlayerIdentifier player_id_;
		};
	}
}