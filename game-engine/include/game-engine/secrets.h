#ifndef GAME_ENGINE_SECRETS_H
#define GAME_ENGINE_SECRETS_H

#include <vector>
#include "secret.h"

namespace GameEngine {

class Secrets
{
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

#endif
