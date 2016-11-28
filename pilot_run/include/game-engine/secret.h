#ifndef GAME_ENGINE_SECRET_H
#define GAME_ENGINE_SECRET_H

#include <functional>

namespace GameEngine {

class Secret
{
	public:
		int card_id;

		bool operator==(const Secret &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			return true;
		}

		bool operator!=(const Secret &rhs) const
		{
			return !(*this == rhs);
		}
};

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Secret> {
		typedef GameEngine::Secret argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);

			return result;
		}
	};

}

#endif
