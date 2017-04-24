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
			auto op = [&](state::CardRef card_ref) {
				if (card_ref == exclude) return;
				if (CheckFilter(manipulate, card_ref)) functor(card_ref);
			};

			if (include_hero) op(player.GetHeroRef());
			if (include_minion) player.minions_.ForEach(op);
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

			if (!include_immune) {
				if (card.GetImmune()) return false;
			}

			switch (filter_type) {
			case kFilterAll:
				return true;
			case kFilterAlive:
				if (card.GetHP() <= 0) return false;
				if (card.GetRawData().pending_destroy) return false;
				return true;
			case kFilterTargetable:
				if (!CheckStealth(card)) return false;
				return true;
			case kFilterTargetableBySpell:
				if (!CheckSpellTargetable(card)) return false;
				return true;
			case kFilterMurloc:
				return (card.GetRace() == kCardRaceMurloc);
			case kFilterBeast:
				return (card.GetRace() == kCardRaceBeast);
			case kFilterPirate:
				return (card.GetRace() == kCardRacePirate);
			case kFilterDemon:
				return (card.GetRace() == kCardRaceDemon);
			case kFilterTargetableBySpellAndDemon:
				if (!CheckSpellTargetable(card)) return false;
				return (card.GetRace() == kCardRaceDemon);
			case kFilterAttackGreaterOrEqualTo:
				return (card.GetAttack() >= filter_arg1);
			case kFilterAttackLessOrEqualTo:
				return (card.GetAttack() <= filter_arg1);
			case kFilterTargetableBySpellAndAttackGreaterOrEqualTo:
				if (!CheckSpellTargetable(card)) return false;
				return (card.GetAttack() >= filter_arg1);
			case kFilterTargetableBySpellAndAttackLessOrEqualTo:
				if (!CheckSpellTargetable(card)) return false;
				return (card.GetAttack() <= filter_arg1);
			case kFilterTaunt:
				return card.HasTaunt();
			case kFilterCharge:
				return card.HasCharge();
			case kFilterUnDamaged:
				return (card.GetDamage() <= 0);
			case kFilterDamaged:
				return (card.GetDamage() > 0);
			}

			assert(false);
			return false; // fallback
		}
	}
}