#pragma once

#include "state/IRandomGenerator.h"
#include "state/Cards/CardData.h"
#include "FlowControl/FlowContext.h"
#include "Cards/id-map.h"

namespace state {
	class State;
}

namespace FlowControl
{
	namespace Manipulators
	{
		class BoardManipulator
		{
		public:
			BoardManipulator(state::State & state, FlowContext & flow_context)
				: state_(state), flow_context_(flow_context)
			{
			}

			state::Cards::CardData GenerateCard(Cards::CardId card_id, state::PlayerIdentifier player);

			void SummonMinion(state::Cards::CardData&& card_data, int pos);

			int GetSpellDamage(state::PlayerIdentifier player);

			void CalculateFinalDamageAmount(state::CardRef source, state::Cards::Card const& source_card, int amount, int * final_amount);

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}