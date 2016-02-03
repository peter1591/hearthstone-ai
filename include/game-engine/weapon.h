#ifndef GAME_ENGINE_WEAPON_H
#define GAME_ENGINE_WEAPON_H

#include <functional>

namespace GameEngine {

class Weapon
{
	public:
		int card_id;

		bool operator==(const Weapon &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			return true;
		}

		bool operator!=(const Weapon &rhs) const
		{
			return !(*this == rhs);
		}
};

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Weapon> {
		typedef GameEngine::Weapon argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);

			return result;
		}
	};

}

#endif
