#pragma once

#include "state/targetor/Targets.h"
#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace state {
	namespace targetor {
		inline void Targets::Fill(state::State const& state, std::vector<CardRef>& targets) const {
			Process(state, [&](CardRef ref) {
				targets.push_back(ref);
				return true;
			});
		}
		inline void Targets::Fill(state::State const& state, std::unordered_set<CardRef>& targets) const {
			Process(state, [&](CardRef ref) {
				targets.insert(ref);
				return true;
			});
		}

		template <typename Functor>
		inline void Targets::ForEach(state::State const& state, Functor&& func) const {
			Process(state, [&](CardRef ref) {
				return func(ref);
			});
		}

		inline void Targets::Count(state::State const& state, int * count) const {
			Process(state, [count](CardRef) {
				++(*count);
				return true;
			});
		}

		template <typename Functor>
		inline void Targets::Process(state::State const& state, Functor&& functor) const {
			if (include_first) {
				if (!ProcessPlayerTargets(state, state.GetBoard().GetFirst(), std::forward<Functor>(functor))) return;
			}
			if (include_second) {
				if (!ProcessPlayerTargets(state, state.GetBoard().GetSecond(), std::forward<Functor>(functor))) return;
			}
		}

		template <typename Functor>
		inline bool Targets::ProcessPlayerTargets(state::State const& state, board::Player const& player, Functor&& functor) const {
			auto op = [&](state::CardRef card_ref) {
				if (card_ref == exclude) return true;
				auto const& card = state.GetCard(card_ref);
				if (!CheckTargetableFilter(state, card)) return true;
				if (!CheckFilter(state, card)) return true;
				return functor(card_ref);
			};

			if (include_hero) {
				if (!op(player.GetHeroRef())) return false;
			}
			if (include_minion) {
				if (!player.minions_.ForEach(op)) return false;
			}
			return true;
		}

		inline bool Targets::CheckTargetable(state::Cards::Card const & card) const
		{
			if (card.GetPlayerIdentifier() != targeting_side) {
				if (card.HasStealth()) return false;
				if (card.GetImmune()) return false;
			}
			return true;
		}

		inline bool Targets::CheckTargetableFilter(state::State const& state, state::Cards::Card const& card) const {
			switch (targetable_type) {
			case kTargetableTypeAll:
				return true;
			case kTargetableTypeOnlyTargetable:
				return CheckTargetable(card);
			case kTargetableTypeOnlySpellTargetable:
				if (card.IsImmuneToSpell()) return false;
				return CheckTargetable(card);
			}

			assert(false);
			return false;
		}
		
		inline bool Targets::CheckFilter(state::State const& state, state::Cards::Card const& card) const {
			switch (filter_type) {
			case kFilterAll:
				return true;
			case kFilterAlive:
				if (card.GetHP() <= 0) return false;
				if (card.GetRawData().pending_destroy) return false;
				return true;
			case kFilterTargetable:
				if (!CheckTargetable(card)) return false;
				return true;
			case kFilterMurloc:
				return (card.GetRace() == kCardRaceMurloc);
			case kFilterBeast:
				return (card.GetRace() == kCardRaceBeast);
			case kFilterPirate:
				return (card.GetRace() == kCardRacePirate);
			case kFilterDemon:
				return (card.GetRace() == kCardRaceDemon);
			case kFilterAttackGreaterOrEqualTo:
				return (card.GetAttack() >= filter_arg1);
			case kFilterAttackLessOrEqualTo:
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