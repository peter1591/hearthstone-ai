#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"

namespace state
{
	class State;

	namespace board
	{
		class OrderedCardsManager
		{
		public:
			typedef std::vector<CardRef> ContainerType;

			explicit OrderedCardsManager(ContainerType & container) : container_(container) {}

			void Insert(State & state, CardRef card_ref);
			void Remove(State & state, size_t pos);

		private:
			ContainerType & container_;
		};
	}
}