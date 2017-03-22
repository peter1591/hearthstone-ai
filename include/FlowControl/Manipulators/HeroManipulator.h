#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CharacterManipulator(state, flow_context, card_ref, card), player_id_(card.GetPlayerIdentifier())
			{
				assert(card.GetCardType() == state::kCardTypeHero);
			}

			void DrawCard();
			void DestroyWeapon();

			void GainArmor(int amount);

			template <state::CardZone KnownZone>
			void EquipWeapon(state::CardRef weapon_ref);

			void TurnStart()
			{
				card_.ClearNumAttacksThisTurn();
			}

		private:
			state::PlayerIdentifier player_id_;
		};
	}
}