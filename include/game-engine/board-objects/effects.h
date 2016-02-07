#pragma once

#include <list>
#include "minion-stat.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

	class Effect
	{
		friend std::hash<Effect>;
		
	public:
		enum Type
		{
			VALID_UNTIL_REMOVED,
			VALID_THIS_TURN
		};

	public:
		static Effect CreateEffect(Type type, MinionStat effect_stat);

		bool operator==(Effect const& rhs) const;
		bool operator!=(Effect const& rhs) const;

	public:
		// return true if modified
		void UpdateStat(MinionStat & stat) const;

	public:
		// return true if this effect still valid,
		// return false if this effect vanished
		bool TurnEnd();

	private:
		Effect() {}

		Type type;
		MinionStat effect_stat;
	};

	class RegisteredEffect
	{
	public:
		typedef std::list<Effect> container_type;

	public:
		RegisteredEffect(container_type & container, container_type::iterator it) : container(container), it(it) {}
		
		void Remove() { this->container.erase(it); }

	private:
		container_type & container;
		container_type::iterator it;
	};

	class Effects
	{
		friend std::hash<Effects>;

	public:
		typedef RegisteredEffect::container_type container_type;

	public:
		bool operator==(Effects const& rhs) const;
		bool operator!=(Effects const& rhs) const;

	public:
		void Add(Effect && effect)
		{
			this->effects.push_back(std::move(effect));
		}

		RegisteredEffect AddRemovableEffect(Effect && effect)
		{
			this->effects.push_back(std::move(effect));
			
			auto it = this->effects.end();
			it--;

			return RegisteredEffect(this->effects, it);
		}

		// return true if modified
		void UpdateStat(MinionStat & stat) const
		{
			for (auto const& effect : this->effects)
			{
				effect.UpdateStat(stat);
			}
		}

	public: // hooks
		void TurnEnd()
		{
			for (auto it = this->effects.begin(); it != this->effects.end();)
			{
				if (it->TurnEnd() == false) {
					// effect vanished
					it = this->effects.erase(it);
				}
				else {
					++it;
				}
			}
		}

	private:
		container_type effects;
	};

	inline Effect Effect::CreateEffect(Type type, MinionStat effect_stat)
	{
		Effect effect;
		effect.type = type;
		effect.effect_stat = effect_stat;
		return effect;
	}

	inline bool Effect::operator==(Effect const & rhs) const
	{
		return true;
	}

	inline bool Effect::operator!=(Effect const & rhs) const
	{
		return !(*this == rhs);
	}

	inline void Effect::UpdateStat(MinionStat & stat) const
	{
		stat.SetAttack(stat.GetAttack() + this->effect_stat.GetAttack());
		stat.SetHP(stat.GetHP() + this->effect_stat.GetHP());
		stat.SetMaxHP(stat.GetMaxHP() + this->effect_stat.GetMaxHP());
		stat.MergeFlags(stat);
	}

	// return true if this effect still valid,
	// return false if this effect vanished
	inline bool Effect::TurnEnd()
	{
		if (this->type == Effect::VALID_THIS_TURN) return false;

		return true;
	}

	inline bool Effects::operator==(Effects const & rhs) const
	{
		if (this->effects != rhs.effects) return false;

		return true;
	}

	inline bool Effects::operator!=(Effects const & rhs) const
	{
		return !(*this == rhs);
	}

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Effect> {
		typedef GameEngine::BoardObjects::Effect argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			return result;
		}
	};

	template <> struct hash<GameEngine::BoardObjects::Effects> {
		typedef GameEngine::BoardObjects::Effects argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;
			
			for (auto const& effect: s.effects) 
				GameEngine::hash_combine(result, effect);

			return result;
		}
	};
}