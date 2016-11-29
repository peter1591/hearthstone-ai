#pragma once

#include "FlowControl/FlowContext.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Helpers
	{
		class AuraUpdater
		{
		public:
			AuraUpdater(state::State & state, FlowContext & flow_context)
				: state_(state), flow_context_(flow_context)
			{
			}

			void Update()
			{
				// update all entities which might omit aura

				for (int i = 0; i < state_.board.Get(state::kPlayerFirst).minions_.Size(); ++i) {
					auto minion_ref = state_.board.Get(state::kPlayerFirst).minions_.Get(i);
					Manipulate(state_, flow_context_).Minion(minion_ref).Aura().Update();
				}

				for (int i = 0; i < state_.board.Get(state::kPlayerSecond).minions_.Size(); ++i) {
					auto minion_ref = state_.board.Get(state::kPlayerSecond).minions_.Get(i);
					Manipulate(state_, flow_context_).Minion(minion_ref).Aura().Update();
				}

				// TODO: weapon can have aura?
			}

		private:
			state::State & state_;
			FlowContext flow_context_;
		};
	}
}