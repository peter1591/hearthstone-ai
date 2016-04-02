#ifndef GAME_ENGINE_BOARD_OBJECTS_WEAPON_H
#define GAME_ENGINE_BOARD_OBJECTS_WEAPON_H

#include <list>
#include <functional>
#include "game-engine/on-death-trigger.h"
#include "game-engine/common.h"

namespace GameEngine {
	class Hero;

	class Weapon
	{
	public:
		typedef GameEngine::OnDeathTrigger<GameEngine::Hero &> OnDeathTrigger;

	public:
		Weapon() : card_id(-1) {}

		bool IsValid() const { return this->card_id > 0; }
		void InValidate() { this->card_id = -1; }

		void Clear() {
			this->InValidate();
			this->forgetful = 0;
			this->freeze_attack = false;
			this->windfury = false;
		}

		bool operator==(const Weapon &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			if (this->cost != rhs.cost) return false;
			if (this->attack != rhs.attack) return false;
			if (this->durability != rhs.durability) return false;
			if (this->forgetful != rhs.forgetful) return false;
			if (this->freeze_attack != rhs.freeze_attack) return false;
			if (this->windfury != rhs.windfury) return false;
			if (this->on_death_triggers != rhs.on_death_triggers) return false;
			return true;
		}

		bool operator!=(const Weapon &rhs) const
		{
			return !(*this == rhs);
		}

	public:
		int card_id;
		int cost;
		int attack;
		int durability;
		int forgetful;
		bool freeze_attack; // freeze the attacked target?
		bool windfury;

		std::list<OnDeathTrigger> on_death_triggers;
	};
} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Weapon> {
		typedef GameEngine::Weapon argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			GameEngine::hash_combine(result, s.cost);
			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.durability);
			GameEngine::hash_combine(result, s.forgetful);
			GameEngine::hash_combine(result, s.freeze_attack);
			GameEngine::hash_combine(result, s.windfury);

			for (auto const& trigger : s.on_death_triggers) {
				GameEngine::hash_combine(result, trigger);
			}

			return result;
		}
	};

}

#endif
