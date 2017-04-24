#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include "state/Types.h"
#include "state/targetor/Targets.h"

namespace state {
	namespace targetor {
		class TargetsGenerator_Leaf
		{
		public:
			TargetsGenerator_Leaf(state::PlayerIdentifier targeting_player) : info_(targeting_player) {}

			Targets const& GetInfo() const { return info_; }

		protected:
			Targets info_;
		};

		class TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter : public TargetsGenerator_Leaf
		{
		public: // Fluent-like API to set up
			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter(state::PlayerIdentifier targeting_player) :
				TargetsGenerator_Leaf(targeting_player)
			{}

			TargetsGenerator_Leaf & Exclude(CardRef ref)
			{
				assert(info_.exclude.IsValid() == false); // only support one exclusion
				info_.exclude = ref;
				return *this;
			}
		};

		class TargetsGenerator_ChosenSide_ChosenHeroMinion : public TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter
		{
		public: // Fluent-like API to set up
			TargetsGenerator_ChosenSide_ChosenHeroMinion(state::PlayerIdentifier targeting_player) :
				TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter(targeting_player)
			{}

			TargetsGenerator_ChosenSide_ChosenHeroMinion & ExcludeImmune() {
				info_.include_immune = false;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Targetable()
			{
				info_.filter_type = Targets::kFilterTargetable;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Alive()
			{
				info_.filter_type = Targets::kFilterAlive;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & SpellTargetable()
			{
				info_.filter_type = Targets::kFilterTargetableBySpell;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Murlocs()
			{
				info_.filter_type = Targets::kFilterMurloc;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Beasts()
			{
				info_.filter_type = Targets::kFilterBeast;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Pirates()
			{
				info_.filter_type = Targets::kFilterPirate;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Demons()
			{
				info_.filter_type = Targets::kFilterDemon;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & SpellTargetableDemons()
			{
				info_.filter_type = Targets::kFilterTargetableBySpellAndDemon;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & AttackGreaterOrEqualTo(int v)
			{
				info_.filter_type = Targets::kFilterAttackGreaterOrEqualTo;
				info_.filter_arg1 = v;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & AttackLessOrEqualTo(int v)
			{
				info_.filter_type = Targets::kFilterAttackLessOrEqualTo;
				info_.filter_arg1 = v;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & SpellTargetableAndAttackGreaterOrEqualTo(int v)
			{
				info_.filter_type = Targets::kFilterTargetableBySpellAndAttackGreaterOrEqualTo;
				info_.filter_arg1 = v;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & SpellTargetableAndAttackLessOrEqualTo(int v)
			{
				info_.filter_type = Targets::kFilterTargetableBySpellAndAttackLessOrEqualTo;
				info_.filter_arg1 = v;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & HasTaunt()
			{
				info_.filter_type = Targets::kFilterTaunt;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & HasCharge()
			{
				info_.filter_type = Targets::kFilterCharge;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & UnDamaged()
			{
				info_.filter_type = Targets::kFilterUnDamaged;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Damaged()
			{
				info_.filter_type = Targets::kFilterDamaged;
				return *this;
			}
		};

		class TargetsGenerator_ChosenSide : public TargetsGenerator_ChosenSide_ChosenHeroMinion
		{
		public: // Fluent-like API to set up
			TargetsGenerator_ChosenSide(state::PlayerIdentifier targeting_player) : TargetsGenerator_ChosenSide_ChosenHeroMinion(targeting_player) {}

			TargetsGenerator_ChosenSide_ChosenHeroMinion & Minion()
			{
				assert(info_.include_minion == true);
				info_.include_hero = false;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion & Hero()
			{
				assert(info_.include_hero == true);
				info_.include_minion = false;
				return *this;
			}
		};

		class TargetsGenerator : public TargetsGenerator_ChosenSide
		{
		public: // Fluent-like API to set up
			TargetsGenerator(state::PlayerIdentifier targeting_player) : TargetsGenerator_ChosenSide(targeting_player) {}

			TargetsGenerator_ChosenSide & Ally() {
				return SideOnly(info_.GetTargetingSide());
			}
			TargetsGenerator_ChosenSide & Enemy() {
				return SideOnly(info_.GetTargetingSide().Opposite());
			}

		private:
			TargetsGenerator_ChosenSide & SideOnly(PlayerIdentifier player)
			{
				if (player.IsFirst()) {
					assert(info_.include_second);
					info_.include_first = true;
					info_.include_second = false;
				}
				else {
					assert(info_.include_first);
					info_.include_first = false;
					info_.include_second = true;
				}
				return *this;
			}
		};
	}
}