#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include <State/Types.h>

namespace Cards
{
	class TargetorInfo
	{
	private:
		template <typename Container>
		class FunctorFillTargets
		{
		public:
			FunctorFillTargets(Container& targets) : targets_(targets) {}

			void operator()(state::CardRef ref) const;

		private:
			Container& targets_;
		};

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
			FunctorFillTargets<Container> functor(targets);

			if (include_first) AddPlayerTargets(state, state.board.Get(state::kPlayerFirst), functor);
			if (include_second) AddPlayerTargets(state, state.board.Get(state::kPlayerSecond), functor);
		}

	private:
		template <typename Functor>
		void AddPlayerTargets(state::State const& state, state::board::Player const& player, Functor const& functor) const
		{
			if (include_hero) {
				if (player.hero_ref_ != exclude) {
					functor(player.hero_ref_);
				}
			}
			if (include_minion) {
				for (state::CardRef minion : player.minions_.Get()) {
					AddMinionTargets(state, minion, functor);
				}
			}
		}

		template <typename Functor>
		void AddMinionTargets(state::State const& state, state::CardRef const& minion, Functor const& functor) const
		{
			if (minion == exclude) return;

			auto const& card = state.mgr.Get(minion);

			switch (minion_filter) {
			case kMinionFilterAll:
				break;
			case kMinionFilterTargetable:
				// TODO: check stealth
				// check owning player; stealth cannot be targeted by enemy
				break;
			case kMinionFilterTargetableBySpell:
				// TODO: check stealh
				// TODO: check immune spell
				break;
			case kMinionFilterMurloc:
				if (card.GetRace() == state::kCardRaceMurloc) break;
				return;
			}

			functor(minion);
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

	template <>
	inline void TargetorInfo::FunctorFillTargets<std::vector<state::CardRef>>::operator()(state::CardRef ref) const
	{
		targets_.push_back(ref);
	}

	template <>
	inline void TargetorInfo::FunctorFillTargets<std::unordered_set<state::CardRef>>::operator()(state::CardRef ref) const
	{
		targets_.insert(ref);
	}

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