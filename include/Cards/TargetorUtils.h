#pragma once

#include <assert.h>
#include <vector>
#include <State/Types.h>

namespace Cards
{
	class TargetorInfo
	{
	public:
		TargetorInfo() :
			include_friendly(true), include_enemy(true),
			include_hero(true), include_minion(true),
			include_stealth(true), include_spell_immune(true)
		{
		}

		std::vector<state::CardRef> GetTargets(state::State const& state) const
		{
			std::vector<state::CardRef> targets;

			if (include_friendly) AddPlayerTargets(state.GetCurrentPlayer(), targets);
			if (include_enemy) AddPlayerTargets(state.GetCurrentPlayer(), targets);

			return targets;
		}

	private:
		void AddPlayerTargets(state::board::Player const& player, std::vector<state::CardRef> & targets) const
		{
			if (include_hero) targets.push_back(player.hero_ref_);
			if (include_minion) {
				for (state::CardRef minion : player.minions_.Get()) {
					AddMinionTargets(minion, targets);
				}
			}
		}

		void AddMinionTargets(state::CardRef const& minion, std::vector<state::CardRef> & targets) const
		{
			// TODO: check stealth
			// TODO: check spell immune
			targets.push_back(minion);
		}

	public:
		bool include_friendly;
		bool include_enemy;

		bool include_minion;
		bool include_hero;

		bool include_stealth;
		bool include_spell_immune;
	};

	class TargetorHelper
	{
	public: // Fluent-like API to set up
		TargetorHelper & Targetable()
		{
			info.include_stealth = false;
			assert(info.include_spell_immune);
			return *this;
		}

		TargetorHelper & SpellTargetable()
		{
			info.include_spell_immune = false;
			info.include_stealth = false;
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