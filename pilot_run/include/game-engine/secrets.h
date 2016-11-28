#ifndef GAME_ENGINE_SECRETS_H
#define GAME_ENGINE_SECRETS_H

#include <functional>
#include <vector>
#include "secret.h"

namespace GameEngine {

class Secrets
{
	friend std::hash<Secrets>;

	public:
		Secrets();

		bool operator==(const Secrets &rhs) const {
			if (this->secrets != rhs.secrets) return false;
			return true;
		}

		bool operator!=(const Secrets &rhs) const {
			return !(*this == rhs);
		}

	private:
		std::vector<Secret> secrets;
};

inline Secrets::Secrets()
{
	this->secrets.reserve(5);
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Secrets> {
		typedef GameEngine::Secrets argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto secret: s.secrets) {
				GameEngine::hash_combine(result, secret);
			}

			return result;
		}
	};

}

#endif
