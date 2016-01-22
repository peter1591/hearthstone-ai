#ifndef GAME_ENGINE_HIDDEN_SECRETS_H
#define GAME_ENGINE_HIDDEN_SECRETS_H

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

#endif
