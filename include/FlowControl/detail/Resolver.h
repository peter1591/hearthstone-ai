#pragma once

#include <unordered_set>
#include <map>
#include "FlowControl/Result.h"

namespace state {
	class State;
}

namespace FlowControl
{
	class FlowContext;

	namespace detail
	{
		class Resolver
		{
		public:
			Resolver() : deaths_() {}

			bool Resolve(state::State & state, FlowContext & flow_context);

		private:
			void CreateDeaths(state::State & state, FlowContext & flow_context);
			bool RemoveDeaths(state::State & state, FlowContext & flow_context);
			bool SetResult(FlowContext & flow_context, Result result);

		private:
			void UpdateAura(state::State & state, FlowContext & flow_context);
			void UpdateEnchantments(state::State & state, FlowContext & flow_context);
			void UpdateEnchantments(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);

		private:
			std::unordered_set<state::CardRef> deaths_;
		};
	}
}