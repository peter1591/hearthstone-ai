#pragma once

namespace FlowControl { class Manipulate; }

namespace state
{
	namespace Events { class Manager; }

	namespace Cards
	{
		struct ZoneChangedContext {
			FlowControl::Manipulate & manipulate_;
			Events::Manager & event_mgr_;
			CardRef card_ref_;
			const Cards::Card & card_;
		};

		typedef void AddedToPlayZoneCallback(ZoneChangedContext&&);
		typedef void AddedToDeckZoneCallback(ZoneChangedContext&&);
	}
}