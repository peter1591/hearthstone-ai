#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include <State/Types.h>

namespace Cards
{
	class TargetorInfo
	{
	public:
		TargetorInfo() :
			include_first(true), include_second(true),
			include_hero(true), include_minion(true),
			minion_filter(kMinionFilterAll)
		{
		}

		template <typename Container>
		void FillTargets(state::State const& state, Container& targets) const
		{
			if (include_first) AddPlayerTargets(state, state.board.Get(state::kPlayerFirst), targets);
			if (include_second) AddPlayerTargets(state, state.board.Get(state::kPlayerSecond), targets);
		}

	private:
		template <typename Container> void AddTarget(state::CardRef ref, Container & targets) const;
		template <> void AddTarget(state::CardRef ref, std::vector<state::CardRef> & targets) const {
			if (ref == exclude) return;
			targets.push_back(ref);
		}
		template <> void AddTarget(state::CardRef ref, std::unordered_set<state::CardRef> & targets) const {
			if (ref == exclude) return;
			targets.insert(ref);
		}

		template <typename Container>
		void AddPlayerTargets(state::State const& state, state::board::Player const& player, Container & targets) const
		{
			if (include_hero) AddTarget(player.hero_ref_, targets);
			if (include_minion) {
				for (state::CardRef minion : player.minions_.Get()) {
					AddMinionTargets(state, minion, targets);
				}
			}
		}

		template <typename Container>
		void AddMinionTargets(state::State const& state, state::CardRef const& minion, Container& targets) const
		{
			auto const& card = state.mgr.Get(minion);

			switch (minion_filter) {
			case kMinionFilterAll:
				break;
			case kMinionFilterTargetable:
				// TODO: check stealth
				break;
			case kMinionFilterTargetableBySpell:
				// TODO: check stealh
				// TODO: check immune spell
				break;
			case kMinionFilterMurloc:
				if (card.GetRace() == state::kCardRaceMurloc) break;
				return;
			}

			AddTarget(minion, targets);
		}

	public:
		bool include_first;
		bool include_second;

		bool include_minion;
		bool include_hero;

		enum MinionFilterType
		{
			kMinionFilterAll,
			kMinionFilterTargetable,
			kMinionFilterTargetableBySpell,
			kMinionFilterMurloc
		} minion_filter;

		state::CardRef exclude;
	};

	class TargetorHelper
	{
	public: // Fluent-like API to set up
		TargetorHelper & Targetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetable;
			return *this;
		}

		TargetorHelper & SpellTargetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetableBySpell;
			return *this;
		}

		TargetorHelper & Murlocs()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterMurloc;
			return *this;
		}

		template <typename Context>
		TargetorHelper & Owner(Context&& context)
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
			if (player == state::kPlayerFirst) {
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
			if (player == state::kPlayerFirst) return Player(state::kPlayerSecond);
			else return Player(state::kPlayerFirst);
		}

	public:
		TargetorInfo GetInfo() const { return info_; }

	private:
		TargetorInfo info_;
	};
}