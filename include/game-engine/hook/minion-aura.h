#pragma once

#include <stdexcept>

#include "game-engine/hook/listener.h"

namespace GameEngine
{
	class Minion;

	// An aura hold by a minion
	// An aura is actually a hook listener
	class MinionAura : HookListener
	{
	public:
		MinionAura(Minion & owner) : owner(owner) {}

	private:
		Minion & owner;
	};

} // namespace GameEngine