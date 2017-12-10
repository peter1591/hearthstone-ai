#pragma once

namespace state
{
	class State;

	namespace Cards
	{
		struct ZoneChangedContext {
			state::State & state_;
			CardRef card_ref_;
		};

		typedef void AddedToPlayZoneCallback(ZoneChangedContext const&);
		typedef void AddedToHandZoneCallback(ZoneChangedContext const&);
	}
}