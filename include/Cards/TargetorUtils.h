#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include "state/Types.h"
#include "state/TargetorInfo.h"

namespace Cards
{
	class TargetorHelper
	{
	public: // Fluent-like API to set up
		TargetorHelper & Targetable()
		{
			info_.minion_filter = state::TargetorInfo::kMinionFilterTargetable;
			return *this;
		}

		TargetorHelper & SpellTargetable()
		{
			info_.minion_filter = state::TargetorInfo::kMinionFilterTargetableBySpell;
			return *this;
		}

		TargetorHelper & Murlocs()
		{
			info_.minion_filter = state::TargetorInfo::kMinionFilterMurloc;
			return *this;
		}

		template <typename Context>
		TargetorHelper & Ally(Context&& context)
		{
			return Player(context.card_.GetPlayerIdentifier());
		}

		template <typename Context>
		TargetorHelper & Enemy(Context&& context)
		{
			return AnotherPlayer(context.card_.GetPlayerIdentifier());
		}

		TargetorHelper & Minion()
		{
			info_.include_hero = false;
			assert(info_.include_minion == true);
			return *this;
		}

		TargetorHelper & Hero()
		{
			info_.include_minion = false;
			assert(info_.include_hero == true);
			return *this;
		}

		TargetorHelper & Exclude(state::CardRef ref)
		{
			assert(info_.exclude.IsValid() == false); // only support one exclusion
			info_.exclude = ref;
			return *this;
		}

	private:
		TargetorHelper & Player(state::PlayerIdentifier player)
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

		TargetorHelper & AnotherPlayer(state::PlayerIdentifier player)
		{
			return Player(player.Opposite());
		}

	public:
		state::TargetorInfo const& GetInfo() const { return info_; }

	private:
		state::TargetorInfo info_;
	};
}