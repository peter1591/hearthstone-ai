#ifndef GAME_ENGINE_BOARD_OBJECTS_HERO_POWER_H
#define GAME_ENGINE_BOARD_OBJECTS_HERO_POWER_H

#include <functional>
#include "game-engine/common.h"

namespace GameEngine {
	struct HeroPower
	{
		HeroPower() : card_id(-1), cost(0), used_this_turn(false) {}

		int card_id;
		int cost;
		bool used_this_turn;

		bool IsValid() const { return this->card_id > 0; }

		bool operator==(HeroPower const& rhs) const
		{
			if (this->IsValid() != rhs.IsValid()) return false;

			if (this->IsValid()) {
				if (this->card_id != rhs.card_id) return false;
				if (this->cost != rhs.cost) return false;
				if (this->used_this_turn != rhs.used_this_turn) return false;
			}

			return true;
		}

		bool operator!=(const HeroPower &rhs) const
		{
			return !(*this == rhs);
		}
	};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::HeroPower> {
		typedef GameEngine::HeroPower argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.IsValid());

			if (s.IsValid()) {
				GameEngine::hash_combine(result, s.card_id);
				GameEngine::hash_combine(result, s.cost);
				GameEngine::hash_combine(result, s.used_this_turn);
			}

			return result;
		}
	};
}

#endif
