#pragma once

#include <vector>
#include <unordered_set>
#include "state/State.h"

namespace state {
	class State;
	class FlowContext;

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

			void Fill(State const& state, std::vector<CardRef>& targets) const
			{
				Process(state, [&](CardRef ref) {
					targets.push_back(ref);
				});
			}
			void Fill(State const& state, std::unordered_set<CardRef>& targets) const
			{
				Process(state, [&](CardRef ref) {
					targets.insert(ref);
				});
			}

			template <typename Functor>
			void ForEach(State & state, FlowContext & flow_context, Functor&& func) const
			{
				Process(state, [&](CardRef ref) {
					func(state, flow_context, ref);
				});
			}

			void Count(State const& state, int * count) const
			{
				Process(state, [count](CardRef) {
					++(*count);
				});
			}

		private:
			template <typename Functor>
			void Process(State const& state, Functor&& functor) const
			{
				if (include_first) ProcessPlayerTargets(state, state.GetBoard().GetFirst(), std::forward<Functor>(functor));
				if (include_second) ProcessPlayerTargets(state, state.GetBoard().GetSecond(), std::forward<Functor>(functor));
			}

			template <typename Functor>
			void ProcessPlayerTargets(State const& state, board::Player const& player, Functor&& functor) const
			{
				if (include_hero) {
					if (player.GetHeroRef() != exclude) {
						functor(player.GetHeroRef());
					}
				}
				if (include_minion) {
					for (CardRef minion : player.minions_.Get()) {
						ProcessMinionTargets(state, minion, std::forward<Functor>(functor));
					}
				}
			}

			template <typename Functor>
			void ProcessMinionTargets(State const& state, CardRef const& minion, Functor&& functor) const
			{
				if (minion == exclude) return;

				auto const& card = state.GetCardsManager().Get(minion);

				switch (minion_filter) {
				case kMinionFilterAll:
					break;
				case kMinionFilterTargetable:
					// TODO: check stealth
					// check owning player; stealth cannot be targeted by enemy
					break;
				case kMinionFilterTargetableBySpell:
					// TODO: check stealh
					// TODO: check immune spell
					break;
				case kMinionFilterMurloc:
					if (card.GetRace() == kCardRaceMurloc) break;
					return;
				}

				functor(minion);
			}

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