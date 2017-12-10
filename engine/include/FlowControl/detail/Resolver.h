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
			class DeathProcessor {
			public:
				DeathProcessor(state::CardRef ref, state::PlayerIdentifier player, state::CardZone zone, int zone_pos, int attack)
					: ref_(ref), player_(player), zone_(zone), zone_pos_(zone_pos), attack_(attack)
				{}

				void operator()(state::State & state, FlowContext & flow_context);

			private:
				state::CardRef ref_;
				state::PlayerIdentifier player_;
				state::CardZone zone_;
				int zone_pos_;
				int attack_;
			};

			bool CreateDeaths(state::State & state, FlowContext & flow_context);
			bool RemoveDeaths(state::State & state, FlowContext & flow_context);
			bool SetResult(FlowContext & flow_context, Result result);

		private:
			void UpdateAura(state::State & state, FlowContext & flow_context);
			void UpdateEnchantments(state::State & state, FlowContext & flow_context);
			void UpdateEnchantments(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);

		private:
			std::vector<state::CardRef> deaths_;
			std::multimap<int, DeathProcessor> ordered_deaths_;
			std::vector<state::CardRef> minions_refs_; // a cache for process
		};
	}
}