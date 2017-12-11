#pragma once

#include "Cards/CardDispatcher.h"

namespace Cards
{
	using TargetsGenerator = state::targetor::TargetsGenerator;
	using Targets = state::targetor::Targets;

	template <typename ClientCard> struct CardClassIdMap;
	template <int CardId> struct CardIdClassMap;

#define REGISTER_CARD_INTERNAL(CardId, CardClass) \
	namespace Cards { template <> struct CardClassIdMap<CardClass> { static constexpr int id = CardId; }; } \
	namespace Cards { template <> struct CardIdClassMap<CardId> { using type = CardClass; }; }

#define REGISTER_CARD(CardId) \
	template <> template <> \
	struct Cards::CardDispatcher::DispatcherImpl::DispatchMap<Cards::ID_ ## CardId> \
	{ typedef Cards::Card_ ## CardId type; }; \
	REGISTER_CARD_INTERNAL(Cards::ID_ ## CardId, Cards::Card_ ## CardId)
}