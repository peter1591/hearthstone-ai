#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

	class Effect
	{
		friend std::hash<Effect>;

	public:
		bool operator==(Effect const& rhs) const;
		bool operator!=(Effect const& rhs) const;

	public: // hooks
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
		RegisteredEffect Add(Effect && effect)
		{
			this->effects.push_back(std::move(effect));
			
			auto it = this->effects.end();
			it--;

			return RegisteredEffect(this->effects, it);
		}

	private:
		container_type effects;
	};

	inline bool Effect::operator==(Effect const & rhs) const
	{
		return true;
	}

	inline bool Effect::operator!=(Effect const & rhs) const
	{
		return !(*this == rhs);
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