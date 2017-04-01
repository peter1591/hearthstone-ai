#pragma once

#include <unordered_set>
#include "Utils/StaticDispatcher.h"
#include "state/State.h"

namespace Cards
{
	namespace detail
	{
		template <int id> class DefaultInvoked;
	}

	class CardDispatcher
	{
	public:
		using DispatcherImpl = Utils::StaticDispatcher<detail::DefaultInvoked>;
		static state::Cards::CardData CreateInstance(int id);
	};
}
