#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include "state/Types.h"
#include "state/utils/Targets.h"

namespace state {
	namespace utils {
		class TargetsGenerator
		{
		public: // Fluent-like API to set up
			TargetsGenerator & Targetable()
			{
				info_.minion_filter = Targets::kMinionFilterTargetable;
				return *this;
			}

			TargetsGenerator & SpellTargetable()
			{
				info_.minion_filter = Targets::kMinionFilterTargetableBySpell;
				return *this;
			}

			TargetsGenerator & Murlocs()
			{
				info_.minion_filter = Targets::kMinionFilterMurloc;
				return *this;
			}

			template <typename Context>
			TargetsGenerator & Ally(Context&& context)
			{
				return Player(context.card_.GetPlayerIdentifier());
			}

			template <typename Context>
			TargetsGenerator & Enemy(Context&& context)
			{
				return AnotherPlayer(context.card_.GetPlayerIdentifier());
			}

			TargetsGenerator & Minion()
			{
				info_.include_hero = false;
				assert(info_.include_minion == true);
				return *this;
			}

			TargetsGenerator & Hero()
			{
				info_.include_minion = false;
				assert(info_.include_hero == true);
				return *this;
			}

			TargetsGenerator & Exclude(CardRef ref)
			{
				assert(info_.exclude.IsValid() == false); // only support one exclusion
				info_.exclude = ref;
				return *this;
			}

		private:
			TargetsGenerator & Player(PlayerIdentifier player)
			{
				if (player.IsFirst()) {
					info_.include_first = true;
					info_.include_second = false;
				}
				else {
					info_.include_first = false;
					info_.include_second = true;
				}
				return *this;
			}

			TargetsGenerator & AnotherPlayer(PlayerIdentifier player)
			{
				return Player(player.Opposite());
			}

		public:
			Targets const& GetInfo() const { return info_; }

		private:
			Targets info_;
		};
	}
}