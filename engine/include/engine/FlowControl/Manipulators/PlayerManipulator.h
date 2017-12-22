#pragma once

#include "Cards/id-map.h"
#include "engine/FlowControl/Manipulators/CharacterManipulator.h"

namespace engine {
	namespace FlowControl
	{
		namespace Manipulators
		{
			class PlayerManipulator
			{
			public:
				PlayerManipulator(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
					: state_(state), flow_context_(flow_context), player_(player)
				{}

				void DiscardHandCard(state::CardRef card_ref);

				template <state::CardZone KnownZone>
				void EquipWeapon(state::CardRef weapon_ref);

				void EquipWeapon(Cards::CardId card_id);

				void DestroyWeapon();

				void ReplaceHeroPower(Cards::CardId id);

				state::CardRef AddHandCard(Cards::CardId card_id);
				state::CardRef DrawCard(Cards::CardId * drawn_card_id = nullptr);

			private:
				state::State & state_;
				FlowContext & flow_context_;
				state::PlayerIdentifier player_;
			};
		}
	}
}