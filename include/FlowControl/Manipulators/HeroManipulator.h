#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/Helpers/WeaponRefRemover.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CharacterManipulator(state, flow_context, card_ref, card), player_id_(card.GetPlayerIdentifier())
			{
				assert(card.GetCardType() == state::kCardTypeHero);
			}

			Helpers::ZoneChangerWithUnknownZone<state::kCardTypeHero> Zone()
			{
				return Helpers::ZoneChangerWithUnknownZone<state::kCardTypeHero>(state_, flow_context_, card_ref_, card_);
			}

			void DrawCard();
			void DestroyWeapon();
			void EquipWeapon(state::CardRef weapon_ref);
			void TurnStart()
			{
				card_.ClearNumAttacksThisTurn();
			}

			// constraint access to resolver only
			Helpers::WeaponRefRemover RemoveWeaponRef()
			{
				return Helpers::WeaponRefRemover(card_);
			}

		private:
			state::PlayerIdentifier player_id_;
		};
	}
}