#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include "state/Types.h"
#include "state/TargetorInfo.h"

namespace state
{
	class TargetorInfoGetter
	{
	public: // Fluent-like API to set up
		TargetorInfoGetter & Targetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetable;
			return *this;
		}

		TargetorInfoGetter & SpellTargetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetableBySpell;
			return *this;
		}

		TargetorInfoGetter & Murlocs()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterMurloc;
			return *this;
		}

		template <typename Context>
		TargetorInfoGetter & Ally(Context&& context)
		{
			return Player(context.card_.GetPlayerIdentifier());
		}

		template <typename Context>
		TargetorInfoGetter & Enemy(Context&& context)
		{
			return AnotherPlayer(context.card_.GetPlayerIdentifier());
		}

		TargetorInfoGetter & Minion()
		{
			info_.include_hero = false;
			assert(info_.include_minion == true);
			return *this;
		}

		TargetorInfoGetter & Hero()
		{
			info_.include_minion = false;
			assert(info_.include_hero == true);
			return *this;
		}

		TargetorInfoGetter & Exclude(CardRef ref)
		{
			assert(info_.exclude.IsValid() == false); // only support one exclusion
			info_.exclude = ref;
			return *this;
		}

	private:
		TargetorInfoGetter & Player(PlayerIdentifier player)
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

		TargetorInfoGetter & AnotherPlayer(PlayerIdentifier player)
		{
			return Player(player.Opposite());
		}

	public:
		TargetorInfo const& GetInfo() const { return info_; }

	private:
		TargetorInfo info_;
	};
}