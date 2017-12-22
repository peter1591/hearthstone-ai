#pragma once

#include "engine/FlowControl/Manipulate.h"

namespace engine {
	namespace FlowControl
	{
		namespace deathrattle
		{
			namespace context
			{
				struct Deathrattle
				{
					Manipulate manipulate_;
					state::CardRef card_ref_;
					state::PlayerIdentifier player_;
					state::CardZone zone_;
					int zone_pos_;
					int attack_;
				};
			}
		}
	}
}