#pragma once

#include "Utils/StaticDispatcher.h"
#include "state/Cards/CardData.h"
#include "Cards/id-map.h"

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
		static state::Cards::CardData CreateInstance(CardId id);
	};
}
