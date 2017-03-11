#pragma once

#include "state/IRandomGenerator.h"

namespace state {
	class State;
	class FlowContext;
}

namespace FlowControl
{
	namespace Manipulators
	{
		class BoardManipulator
		{
		public:
			BoardManipulator(state::State & state, state::FlowContext & flow_context)
				: state_(state), flow_context_(flow_context)
			{
			}

			void Summon(state::CardRef ref);

		private:
			state::State & state_;
			state::FlowContext & flow_context_;
		};
	}
}