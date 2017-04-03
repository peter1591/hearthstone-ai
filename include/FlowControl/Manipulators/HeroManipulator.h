#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref), player_id_(GetCard().GetPlayerIdentifier())
			{
				assert(GetCard().GetCardType() == state::kCardTypeHero);
			}

			void DrawCard();
			void AddHandCard(int card_id);
			void DestroyWeapon();

			void GainArmor(int amount);

			template <state::CardZone KnownZone>
			void EquipWeapon(state::CardRef weapon_ref);

			void TurnStart()
			{
				GetCard().ClearNumAttacksThisTurn();
			}

		private:
			state::PlayerIdentifier player_id_;
		};
	}
}