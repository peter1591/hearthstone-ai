#pragma once

#include <vector>
#include <unordered_set>
#include "state/Types.h"

namespace FlowControl { class Manipulate; }

namespace state {
	class State;
	namespace board { class Player; }

	namespace targetor {
		class Targets
		{
		public:
			Targets() :
				include_first(true), include_second(true),
				include_hero(true), include_minion(true),
				minion_filter(kMinionFilterAll)
			{
			}

			static Targets None() {
				Targets ret;
				ret.include_first = false;
				ret.include_second = false;
				return ret;
			}

		public:
			void Fill(FlowControl::Manipulate & manipulate, std::vector<CardRef>& targets) const;
			void Fill(FlowControl::Manipulate & manipulate, std::unordered_set<CardRef>& targets) const;

			template <typename Functor>
			void ForEach(FlowControl::Manipulate & manipulate, Functor&& func) const;

			void Count(FlowControl::Manipulate & manipulate, int * count) const;

		private:
			template <typename Functor>
			void Process(FlowControl::Manipulate & manipulate, Functor&& functor) const;

			template <typename Functor>
			void ProcessPlayerTargets(FlowControl::Manipulate & manipulate, board::Player const& player, Functor&& functor) const;

			template <typename Functor>
			void ProcessMinionTargets(FlowControl::Manipulate & manipulate, CardRef minion, Functor&& functor) const;

		public:
			bool include_first;
			bool include_second;

			bool include_minion;
			bool include_hero;

			enum MinionFilterType
			{
				kMinionFilterAll,
				kMinionFilterNotMorallyWounded,
				kMinionFilterTargetable,
				kMinionFilterTargetableBySpell,
				kMinionFilterMurloc
			} minion_filter;

			CardRef exclude;
		};
	}
}