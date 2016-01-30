#ifndef GAME_ENGINE_HIDDEN_SECRETS_H
#define GAME_ENGINE_HIDDEN_SECRETS_H

#include <functional>

namespace GameEngine {

struct HiddenSecrets
{
	// TODO

	bool operator==(const HiddenSecrets &) const
	{
		return true;
	}

	bool operator!=(const HiddenSecrets &rhs) const
	{
		return !(*this == rhs);
	}
};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::HiddenSecrets> {
		typedef GameEngine::HiddenSecrets argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &) const {
			result_type result = 0;

			return result;
		}
	};

}

#endif
