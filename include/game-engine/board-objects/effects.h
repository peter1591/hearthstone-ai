#pragma once

#include <iostream>
#include <list>
#include "minion-stat.h"
#include "game-engine/aura.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

	class Effect
	{
		friend std::hash<Effect>;
		
	public:
		enum Type
		{
			TYPE_SELF, // only buff itself
			TYPE_AURA
		};

		enum LifeTime
		{
			VALID_UNTIL_REMOVED,
			VALID_THIS_TURN
		};

	public:
		// create a effect with type 'TYPE_SELF'
		static Effect CreateEffect(LifeTime life_time, MinionStat && effect_stat);

		// craete a effect with type 'TYPE_AURA'
		static Effect CreateEffect(LifeTime life_time, RegisteredAura && aura);

		bool operator==(Effect const& rhs) const;
		bool operator!=(Effect const& rhs) const;

	public:
		// return true if modified
		void UpdateStat(MinionStat & stat) const;

	public: // hooks
		// return true if this effect still valid,
		// return false if this effect vanished
		bool TurnEnd();

		void BeforeRemoved();

	private:
		Effect();

		Type type;

		LifeTime life_time;

		MinionStat effect_stat;
		RegisteredAura aura;
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

		void UpdateStat(MinionStat & stat) const
		{
			for (auto const& effect : this->effects)
			{
				effect.UpdateStat(stat);
			}
		}

		void Clear()
		{
			for (auto it = this->effects.begin(); it != this->effects.end();)
			{
				it = this->Remove(it);
			}
		}

	public: // hooks
		void TurnEnd()
		{
			for (auto it = this->effects.begin(); it != this->effects.end();)
			{
				if (it->TurnEnd() == false) {
					it = this->Remove(it); // effect died
				}
				else {
					++it;
				}
			}
		}

	private:
		container_type::iterator Remove(container_type::iterator it)
		{
			it->BeforeRemoved();
			return this->effects.erase(it);
		}

	private:
		container_type effects;
	};

	inline Effect Effect::CreateEffect(LifeTime life_time, MinionStat && effect_stat)
	{
		Effect effect;
		effect.type = TYPE_SELF;
		effect.life_time = life_time;
		effect.effect_stat = std::move(effect_stat);
		return effect;
	}

	inline Effect Effect::CreateEffect(LifeTime life_time, RegisteredAura && aura)
	{
		Effect effect;
		effect.type = TYPE_AURA;
		effect.life_time = life_time;
		effect.aura = std::move(aura);
		return effect;
	}

	inline bool Effect::operator==(Effect const & rhs) const
	{
		if (this->type != rhs.type) return false;
		if (this->life_time != rhs.life_time) return false;
		
		switch (this->type) {
		case TYPE_SELF:
			if (this->effect_stat != rhs.effect_stat) return false;
			break;

		case TYPE_AURA:
			if (this->aura != rhs.aura) return false;
			break;
		}

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
		if (this->life_time == Effect::VALID_THIS_TURN) return false;

		return true;
	}

	inline void Effect::BeforeRemoved()
	{
		switch (this->type) {
		case TYPE_SELF:
			break;

		case TYPE_AURA:
			this->aura.Remove();
			break;
		}
	}

	inline Effect::Effect()
	{
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

			GameEngine::hash_combine(result, s.type);
			GameEngine::hash_combine(result, s.life_time);

			switch (s.type) {
			case GameEngine::BoardObjects::Effect::TYPE_SELF:
				GameEngine::hash_combine(result, s.effect_stat);
				break;

			case GameEngine::BoardObjects::Effect::TYPE_AURA:
				GameEngine::hash_combine(result, s.aura);
				break;
			}

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