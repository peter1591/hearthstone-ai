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
				Targets()
			{
				Initialize(targeting_side);
			}

			Targets() :
				include_first(true), include_second(true),
				include_hero(true), include_minion(true),
				targetable_type(kTargetableTypeAll),
				filter_type(kFilterAll)
			{}

			void Initialize(state::PlayerIdentifier in_targeting_side) {
				this->targeting_side = in_targeting_side;
			}

			static Targets None() {
				Targets ret(state::PlayerIdentifier::First()); // side is not important
				ret.include_first = false;
				ret.include_second = false;
				return ret;
			}

			state::PlayerIdentifier GetTargetingSide() const { return targeting_side; }

		public:
			void Fill(FlowControl::Manipulate const& manipulate, std::vector<CardRef>& targets) const;
			void Fill(FlowControl::Manipulate const& manipulate, std::unordered_set<CardRef>& targets) const;

			template <typename Functor>
			void ForEach(FlowControl::Manipulate const& manipulate, Functor&& func) const;

			void Count(FlowControl::Manipulate const& manipulate, int * count) const;

		private:
			template <typename Functor>
			void Process(FlowControl::Manipulate const& manipulate, Functor&& functor) const;

			template <typename Functor>
			void ProcessPlayerTargets(FlowControl::Manipulate const& manipulate, board::Player const& player, Functor&& functor) const;

			bool CheckTargetableFilter(FlowControl::Manipulate const& manipulate, state::Cards::Card const& card) const;
			bool CheckFilter(FlowControl::Manipulate const& manipulate, state::Cards::Card const& card) const;

			bool CheckTargetable(state::Cards::Card const& card) const;

		public:
			state::PlayerIdentifier targeting_side; // used to determine if a stealth minion can be targeted

			bool include_first;
			bool include_second;

			bool include_hero;
			bool include_minion;

			enum TargetableType
			{
				kTargetableTypeAll,
				kTargetableTypeOnlyTargetable,
				kTargetableTypeOnlySpellTargetable
			};
			TargetableType targetable_type;

			enum FilterType
			{
				kFilterAll,
				kFilterAlive, // mortally wounded, or marked as pending-destroy
				kFilterTargetable,
				kFilterMurloc,
				kFilterBeast,
				kFilterPirate,
				kFilterDemon,
				kFilterAttackGreaterOrEqualTo,
				kFilterAttackLessOrEqualTo,
				kFilterTaunt,
				kFilterCharge,
				kFilterUnDamaged,
				kFilterDamaged
			} filter_type;
			int filter_arg1;

			CardRef exclude;
		};
	}
}