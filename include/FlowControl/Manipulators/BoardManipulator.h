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

			state::Cards::Card GenerateCard(Cards::CardId card_id, state::PlayerIdentifier player);
			state::Cards::Card GenerateCardByCopy(state::Cards::Card const& card, state::PlayerIdentifier player);

			void SummonMinion(state::Cards::Card card, int pos);

			int GetSpellDamage(state::PlayerIdentifier player);

			void CalculateFinalDamageAmount(state::CardRef source, state::Cards::Card const& source_card, int amount, int * final_amount);

		private:
			state::Cards::Card GenerateCard(state::Cards::CardData card_data, state::PlayerIdentifier player);

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}