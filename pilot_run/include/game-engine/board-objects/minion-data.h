#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <functional>
#include <bitset>
#include "game-engine/card.h"
#include "game-engine/slot-index.h"
#include "object-base.h"
#include "minion-stat.h"
#include "game-engine/on-death-trigger.h"

namespace GameEngine {

	class Board;

	class Minion;
	class MinionIterator;

	class MinionData
	{
		friend std::hash<MinionData>;

	public:
		typedef OnDeathTrigger<MinionIterator &> OnDeathTrigger;

	public:
		MinionData();
		MinionData(int card_id, int attack, int hp, int max_hp, int spell_damage);

		static MinionData FromCard(Card const& card);

		MinionData(MinionData const& rhs)
		{
			*this = rhs;
		}

		// for transformation
		MinionData & operator=(MinionData const& rhs)
		{
			this->card_id = rhs.card_id;
			this->stat = rhs.stat;
			this->attacked_times = rhs.attacked_times;
			this->summoned_this_turn = rhs.summoned_this_turn;
			this->pending_removal = rhs.pending_removal;
			this->triggers_on_death = rhs.triggers_on_death;
			return *this;
		}

		MinionData(MinionData && rhs)
		{
			this->card_id = std::move(rhs.card_id);
			this->stat = std::move(rhs.stat);
			this->attacked_times = std::move(rhs.attacked_times);
			this->summoned_this_turn = std::move(rhs.summoned_this_turn);
			this->pending_removal = std::move(rhs.pending_removal);
			this->triggers_on_death = std::move(rhs.triggers_on_death);
		}
		MinionData & operator=(MinionData && rhs) = delete;

		bool operator==(const MinionData &rhs) const;
		bool operator!=(const MinionData &rhs) const;

		bool IsValid() const { return this->card_id != 0; }

	public:
		std::string GetDebugString() const;

	public:
		int card_id;

		MinionStat stat;

		int attacked_times;
		bool summoned_this_turn;

		// mark as pending death when triggering deathrattles
		bool pending_removal;

		std::list<OnDeathTrigger> triggers_on_death;
	};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::MinionData> {
		typedef GameEngine::MinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			
			GameEngine::hash_combine(result, s.stat);
			
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			GameEngine::hash_combine(result, s.pending_removal);

			for (auto const& trigger : s.triggers_on_death) {
				GameEngine::hash_combine(result, trigger);
			}

			return result;
		}
	};
}