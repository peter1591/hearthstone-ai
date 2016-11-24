#pragma once

#include <utility>
#include "CloneableContainers/Vector.h"
#include "State/Cards/Card.h"
#include "State/CardRef.h"
#include "State/PlayerIdentifier.h"

namespace Manipulators
{
	class StateManipulator;
}

namespace State
{
	class State;

	namespace Cards
	{
		class Manager
		{
			friend class Manipulators::StateManipulator;

		public:
			typedef CloneableContainers::Vector<Card> ContainerType;

			const Card & Get(const CardRef & id) const
			{
				return cards_.Get(id.id);
			}

			template <typename T> CardRef PushBack(State & state, T&& card);

		private:
			Card & GetMutable(const CardRef & id)
			{
				return cards_.Get(id.id);
			}

		private:
			ContainerType cards_;
		};
	}
}