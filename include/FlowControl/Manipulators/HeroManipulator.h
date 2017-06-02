#pragma once

#include "Cards/id-map.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		// TODO: rename to OnBoardHeroManipulator
		// TODO: separate out a PlayerManipulator
		//     Rule of thumb: will the interface be influenced if the hero is changed (e.g., by Jaxx), 
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref), player_id_(GetCard().GetPlayerIdentifier())
			{
				assert(GetCard().GetZone() == state::kCardZonePlay);
				assert(GetCard().GetCardType() == state::kCardTypeHero);
			}
			
			void DiscardHandCard(state::CardRef card_ref);

			void DestroyWeapon();

			void GainArmor(int amount);

			template <state::CardZone KnownZone>
			void EquipWeapon(state::CardRef weapon_ref);

			void EquipWeapon(Cards::CardId card_id);

			void TurnStart()
			{
				GetCard().ClearNumAttacksThisTurn();
			}

			state::CardRef HeroManipulator::Transform(Cards::CardId id);

		private:
			state::PlayerIdentifier player_id_;
		};

		class PlayerManipulator
		{
		public:
			PlayerManipulator(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				: state_(state), flow_context_(flow_context), player_(player)
			{}

			state::CardRef AddHandCard(Cards::CardId card_id);
			state::CardRef DrawCard(Cards::CardId * drawn_card_id = nullptr);

		private:
			state::State & state_;
			FlowContext & flow_context_;
			state::PlayerIdentifier player_;
		};
	}
}