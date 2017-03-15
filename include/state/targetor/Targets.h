#pragma once

#include <vector>
#include <unordered_set>
#include "state/board/Player.h"

namespace FlowControl { class FlowContext; }

namespace state {
	class State;

	namespace targetor {
		class Targets
		{
		public:
			Targets();

			void Fill(State const& state, std::vector<CardRef>& targets) const;
			void Fill(State const& state, std::unordered_set<CardRef>& targets) const;

			template <typename Functor>
			void ForEach(State & state, FlowControl::FlowContext & flow_context, Functor&& func) const;

			void Count(State const& state, int * count) const;

		private:
			template <typename Functor>
			void Process(State const& state, Functor&& functor) const;

			template <typename Functor>
			void ProcessPlayerTargets(State const& state, board::Player const& player, Functor&& functor) const;

			template <typename Functor>
			void ProcessMinionTargets(State const& state, CardRef const& minion, Functor&& functor) const;

		public:
			bool include_first;
			bool include_second;

			bool include_minion;
			bool include_hero;

			enum MinionFilterType
			{
				kMinionFilterAll,
				kMinionFilterTargetable,
				kMinionFilterTargetableBySpell,
				kMinionFilterMurloc
			} minion_filter;

			CardRef exclude;
		};
	}
}