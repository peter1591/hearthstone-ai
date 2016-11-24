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
}

namespace State
{
	namespace Cards
	{
		class Manager
		{
			friend class Manipulators::StateManipulator;

		public:
			typedef CloneableContainers::Vector<Entity::Card> ContainerType;

			const Entity::Card & Get(const CardRef & id) const
			{
				return cards_.Get(id.id);
			}

			template <typename T> CardRef PushBack(State & state, T&& card);

		private:
			Entity::Card & GetMutable(const CardRef & id)
			{
				return cards_.Get(id.id);
			}

		private:
			ContainerType cards_;
		};
	}
}