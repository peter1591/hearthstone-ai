#pragma once

namespace FlowControl { class Manipulate; }

namespace state
{
	namespace Cards
	{
		struct ZoneChangedContext {
			FlowControl::Manipulate & manipulate_;
			CardRef card_ref_;
			const Cards::Card & card_;
		};

		typedef void AddedToPlayZoneCallback(ZoneChangedContext&&);
		typedef void AddedToDeckZoneCallback(ZoneChangedContext&&);
	}
}