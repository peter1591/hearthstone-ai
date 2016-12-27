#pragma once

#include "FlowControl/IRandomGenerator.h"

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

			void Summon(state::CardRef ref);

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}