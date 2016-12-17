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
			include_friendly(true), include_enemy(true),
			include_hero(true), include_minion(true),
			minion_filter(kMinionFilterAll)
		{
		}

		template <typename Container>
		void FillTargets(state::State const& state, Container& targets) const
		{
			if (include_friendly) AddPlayerTargets(state, state.GetCurrentPlayer(), targets);
			if (include_enemy) AddPlayerTargets(state, state.GetOppositePlayer(), targets);
		}

	private:
		template <typename Container> static void AddTarget(state::CardRef ref, Container & targets);
		template <> static void AddTarget(state::CardRef ref, std::vector<state::CardRef> & targets) {
			targets.push_back(ref);
		}
		template <> static void AddTarget(state::CardRef ref, std::unordered_set<state::CardRef> & targets) {
			targets.insert(ref);
		}

		void AddPlayerTargets(state::State const& state, state::board::Player const& player, std::vector<state::CardRef> & targets) const
		{
			if (include_hero) AddTarget(player.hero_ref_, targets);
			if (include_minion) {
				for (state::CardRef minion : player.minions_.Get()) {
					AddMinionTargets(state, minion, targets);
				}
			}
		}

		void AddMinionTargets(state::State const& state, state::CardRef const& minion, std::vector<state::CardRef> & targets) const
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
		bool include_friendly;
		bool include_enemy;

		bool include_minion;
		bool include_hero;

		enum MinionFilterType
		{
			kMinionFilterAll,
			kMinionFilterTargetable,
			kMinionFilterTargetableBySpell,
			kMinionFilterMurloc
		} minion_filter;
	};

	class TargetorHelper
	{
	public: // Fluent-like API to set up
		TargetorHelper & Targetable()
		{
			info.minion_filter = TargetorInfo::kMinionFilterTargetable;
			return *this;
		}

		TargetorHelper & SpellTargetable()
		{
			info.minion_filter = TargetorInfo::kMinionFilterTargetableBySpell;
			return *this;
		}

		TargetorHelper & Murlocs()
		{
			info.minion_filter = TargetorInfo::kMinionFilterMurloc;
			return *this;
		}

		TargetorHelper & Friendly()
		{
			assert(info.include_friendly == true);
			info.include_enemy = false;
			return *this;
		}

		TargetorHelper & Enemy()
		{
			info.include_friendly = false;
			assert(info.include_enemy == true);
			return *this;
		}

		TargetorHelper & Minion()
		{
			info.include_hero = false;
			assert(info.include_minion == true);
			return *this;
		}

		TargetorHelper & Hero()
		{
			info.include_minion = false;
			assert(info.include_hero == true);
			return *this;
		}

	public:
		TargetorInfo GetInfo() const { return info; }

	private:
		TargetorInfo info;
	};
}