#pragma once

#include "state/targetor/Targets.h"
#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace state {
	namespace targetor {
		inline void Targets::Fill(FlowControl::Manipulate & manipulate, std::vector<CardRef>& targets) const {
			Process(manipulate, [&](CardRef ref) {
				targets.push_back(ref);
			});
		}
		inline void Targets::Fill(FlowControl::Manipulate & manipulate, std::unordered_set<CardRef>& targets) const {
			Process(manipulate, [&](CardRef ref) {
				targets.insert(ref);
			});
		}

		template <typename Functor>
		inline void Targets::ForEach(FlowControl::Manipulate & manipulate, Functor&& func) const {
			Process(manipulate, [&](CardRef ref) {
				func(manipulate, ref);
			});
		}

		inline void Targets::Count(FlowControl::Manipulate & manipulate, int * count) const {
			Process(manipulate, [count](CardRef) {
				++(*count);
			});
		}

		template <typename Functor>
		inline void Targets::Process(FlowControl::Manipulate & manipulate, Functor&& functor) const {
			if (include_first) ProcessPlayerTargets(manipulate, manipulate.Board().FirstPlayer(), std::forward<Functor>(functor));
			if (include_second) ProcessPlayerTargets(manipulate, manipulate.Board().SecondPlayer(), std::forward<Functor>(functor));
		}

		template <typename Functor>
		inline void Targets::ProcessPlayerTargets(FlowControl::Manipulate & manipulate, board::Player const& player, Functor&& functor) const {
			if (include_hero) {
				if (player.GetHeroRef() != exclude) {
					functor(player.GetHeroRef());
				}
			}
			if (include_minion) {
				for (CardRef minion : player.minions_.Get()) {
					if (minion == exclude) continue;
					if (CheckFilter(manipulate, minion)) functor(minion);
				}
			}
		}

		inline bool Targets::CheckStealth(state::Cards::Card const & target) const
		{
			if (!target.HasStealth()) return true;
			if (target.GetPlayerIdentifier() == targeting_side) return true; // owner can still target stealth minions
			return false;
		}

		inline bool Targets::CheckSpellTargetable(state::Cards::Card const & card) const
		{
			if (card.IsImmuneToSpell()) return false;
			if (!CheckStealth(card)) return false;
			return true;
		}

		inline bool Targets::CheckFilter(FlowControl::Manipulate & manipulate, CardRef minion) const {
			auto const& card = manipulate.Board().GetCard(minion);

			switch (minion_filter) {
			case kMinionFilterAll:
				return true;
			case kMinionFilterAlive:
				if (card.GetHP() <= 0) return false;
				if (card.GetRawData().pending_destroy) return false;
				return true;
			case kMinionFilterTargetable:
				if (!CheckStealth(card)) return false;
				return true;
			case kMinionFilterTargetableBySpell:
				if (!CheckSpellTargetable(card)) return false;
				return true;
			case kMinionFilterMurloc:
				return (card.GetRace() == kCardRaceMurloc);
			case kMinionFilterBeast:
				return (card.GetRace() == kCardRaceBeast);
			case kMinionFilterPirate:
				return (card.GetRace() == kCardRacePirate);
			case kMinionFilterMinionAttackGreaterOrEqualTo:
				return (card.GetAttack() >= minion_filter_arg1);
			case kMinionFilterMinionAttackLessOrEqualTo:
				return (card.GetAttack() <= minion_filter_arg1);
			case kMinionFilterTargetableBySpellAndMinionAttackGreaterOrEqualTo:
				if (!CheckSpellTargetable(card)) return false;
				return (card.GetAttack() >= minion_filter_arg1);
			case kMinionFilterTargetableBySpellAndMinionAttackLessOrEqualTo:
				if (!CheckSpellTargetable(card)) return false;
				return (card.GetAttack() <= minion_filter_arg1);
			case kMinionFilterTaunt:
				return card.HasTaunt();
			case kMinionFilterUnDamaged:
				return (card.GetDamage() <= 0);
			}

			assert(false);
			return false; // fallback
		}
	}
}