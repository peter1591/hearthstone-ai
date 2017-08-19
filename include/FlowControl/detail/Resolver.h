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
			Resolver() : deaths_(), ordered_deaths_(), minions_refs_() {}

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
			std::multimap<int, std::function<void()>> ordered_deaths_;
			std::vector<state::CardRef> minions_refs_; // a cache for process
		};
	}
}