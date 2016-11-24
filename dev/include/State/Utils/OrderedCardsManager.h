#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"

namespace State
{
	class State;

	namespace Utils
	{
		class OrderedCardsManager
		{
		public:
			typedef std::vector<CardRef> ContainerType;

			explicit OrderedCardsManager(ContainerType & container) : container_(container) {}

			void Insert(::State::State & state, CardRef card_ref);
			void Remove(::State::State & state, size_t pos);

		private:
			ContainerType & container_;
		};
	}
}