#pragma once

#include "state/Cards/Card.h"
#include "engine/FlowControl/Manipulators/CharacterManipulator.h"
#include "Cards/id-map.h"

namespace engine {
	namespace FlowControl
	{
		namespace Manipulators
		{
			class OnBoardCharacterManipulator : public CharacterManipulator
			{
			public:
				OnBoardCharacterManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
					: CharacterManipulator(state, flow_context, card_ref)
				{
				}

				void Destroy() {
					if (GetCard().GetZone() != state::kCardZonePlay) return;
					GetCard().SetPendingDestroy();
					flow_context_.AddDeadEntryHint(state_, card_ref_);
				}
			};
		}
	}
}