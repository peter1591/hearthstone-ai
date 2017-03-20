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
			Targets const& GetInfo() const { return info_; }

		protected:
			Targets info_;
		};

		class TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter : public TargetsGenerator_Leaf
		{
		public: // Fluent-like API to set up
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
			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Targetable()
			{
				info_.minion_filter = Targets::kMinionFilterTargetable;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & NotMortallyWounded()
			{
				info_.minion_filter = Targets::kMinionFilterNotMorallyWounded;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & SpellTargetable()
			{
				info_.minion_filter = Targets::kMinionFilterTargetableBySpell;
				return *this;
			}

			TargetsGenerator_ChosenSide_ChosenHeroMinion_ChosenMinionFilter & Murlocs()
			{
				info_.minion_filter = Targets::kMinionFilterMurloc;
				return *this;
			}
		};

		class TargetsGenerator_ChosenSide : public TargetsGenerator_ChosenSide_ChosenHeroMinion
		{
		public: // Fluent-like API to set up
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
			template <typename Context>
			TargetsGenerator_ChosenSide & Ally(Context&& context)
			{
				return SideOnly(context.card_.GetPlayerIdentifier());
			}

			template <typename Context>
			TargetsGenerator_ChosenSide & Enemy(Context&& context)
			{
				return SideOnly(context.card_.GetPlayerIdentifier().Opposite());
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