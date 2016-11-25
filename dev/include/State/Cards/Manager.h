#pragma once

#include <utility>
#include "Utils/CloneableContainers/Vector.h"
#include "State/Cards/Card.h"
#include "State/Types.h"

namespace state
{
	class State;
	namespace Manipulators
	{
		class StateManipulator;
	}

	namespace Cards
	{
		class Manager
		{
			friend class Manipulators::StateManipulator;

		public:
			typedef Utils::CloneableContainers::Vector<Card> ContainerType;

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