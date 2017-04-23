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
			Targets(state::PlayerIdentifier targeting_side) :
				targeting_side(targeting_side),
				include_first(true), include_second(true),
				include_hero(true), include_minion(true),
				minion_filter(kMinionFilterAll)
			{
			}

			Targets() :
				include_first(true), include_second(true),
				include_hero(true), include_minion(true),
				minion_filter(kMinionFilterAll)
			{}

			static Targets None() {
				Targets ret(state::PlayerIdentifier::First()); // side is not important
				ret.include_first = false;
				ret.include_second = false;
				return ret;
			}

			state::PlayerIdentifier GetTargetingSide() const { return targeting_side; }

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

			bool CheckStealth(state::Cards::Card const& target) const;

			bool CheckSpellTargetable(state::Cards::Card const& card) const;

		public:
			state::PlayerIdentifier targeting_side; // used to determine if a stealth minion can be targeted

			bool include_first;
			bool include_second;

			bool include_minion;
			bool include_hero;

			enum MinionFilterType // TODO: rename to CharacterFilterType, or just FilterType. Hero with demon race can still be targeted
			{
				kMinionFilterAll,
				kMinionFilterAlive, // mortally wounded, or marked as pending-destroy
				kMinionFilterTargetable,
				kMinionFilterTargetableBySpell,
				kMinionFilterMurloc,
				kMinionFilterBeast,
				kMinionFilterPirate,
				kMinionFilterMinionAttackGreaterOrEqualTo,
				kMinionFilterMinionAttackLessOrEqualTo,
				kMinionFilterTargetableBySpellAndMinionAttackGreaterOrEqualTo,
				kMinionFilterTargetableBySpellAndMinionAttackLessOrEqualTo,
				kMinionFilterTaunt,
				kMinionFilterUnDamaged
			} minion_filter;
			int minion_filter_arg1;

			CardRef exclude;
		};
	}
}