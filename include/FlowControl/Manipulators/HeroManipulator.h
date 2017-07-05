#pragma once

#include "Cards/id-map.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		// TODO: rename to OnBoardHeroManipulator
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref), player_id_(GetCard().GetPlayerIdentifier())
			{
				assert(GetCard().GetZone() == state::kCardZonePlay);
				assert(GetCard().GetCardType() == state::kCardTypeHero);
			}

			void GainArmor(int amount);

			void TurnStart()
			{
				GetCard().ClearNumAttacksThisTurn();
			}

			state::CardRef Transform(Cards::CardId id);

		private:
			state::PlayerIdentifier player_id_;
		};
	}
}