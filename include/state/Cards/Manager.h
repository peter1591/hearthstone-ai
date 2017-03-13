#pragma once

#include <utility>

#include "Utils/CloneableContainers/Vector.h"
#include "state/Cards/Card.h"
#include "state/Types.h"
#include "state/detail/ZonePositionSetter.h"

#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	class FlowContext;
}

namespace state
{
	class State;

	namespace Cards
	{
		class Manager
		{
		public:
			typedef Utils::CloneableContainers::Vector<Card> ContainerType;

			Card const& Get(CardRef id) const { return cards_.Get(id.id); }
			Card & GetMutable(CardRef id) { return cards_.Get(id.id); }

			CardRef PushBack(Cards::Card&& card);

			void SetCardZonePos(CardRef ref, int pos)
			{
				detail::ZonePositionSetter(GetMutable(ref)).Set(pos);
			}

		private:
			ContainerType cards_;
		};
	}
}