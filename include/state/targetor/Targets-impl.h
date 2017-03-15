#pragma once

#include "state/targetor/Targets.h"
#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace state {
	namespace targetor {
		inline Targets::Targets() :
			include_first(true), include_second(true),
			include_hero(true), include_minion(true),
			minion_filter(kMinionFilterAll)
		{
		}

		inline void Targets::Fill(State const& state, std::vector<CardRef>& targets) const
		{
			Process(state, [&](CardRef ref) {
				targets.push_back(ref);
			});
		}
		inline void Targets::Fill(State const& state, std::unordered_set<CardRef>& targets) const
		{
			Process(state, [&](CardRef ref) {
				targets.insert(ref);
			});
		}

		template <typename Functor>
		inline void Targets::ForEach(State & state, FlowControl::FlowContext & flow_context, Functor&& func) const
		{
			Process(state, [&](CardRef ref) {
				func(state, flow_context, ref);
			});
		}

		inline void Targets::Count(State const& state, int * count) const
		{
			Process(state, [count](CardRef) {
				++(*count);
			});
		}

		template <typename Functor>
		inline void Targets::Process(State const& state, Functor&& functor) const
		{
			if (include_first) ProcessPlayerTargets(state, state.GetBoard().GetFirst(), std::forward<Functor>(functor));
			if (include_second) ProcessPlayerTargets(state, state.GetBoard().GetSecond(), std::forward<Functor>(functor));
		}

		template <typename Functor>
		inline void Targets::ProcessPlayerTargets(State const& state, board::Player const& player, Functor&& functor) const
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
		inline void Targets::ProcessMinionTargets(State const& state, CardRef const& minion, Functor&& functor) const
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
	}
}