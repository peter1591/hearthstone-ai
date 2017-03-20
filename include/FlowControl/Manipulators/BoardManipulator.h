#pragma once

#include "state/IRandomGenerator.h"
#include "state/Cards/CardData.h"
#include "FlowControl/FlowContext.h"

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

			void Summon(state::Cards::CardData&& card_data, state::PlayerIdentifier player, int pos);

			int GetSpellDamage(state::PlayerIdentifier player);

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}