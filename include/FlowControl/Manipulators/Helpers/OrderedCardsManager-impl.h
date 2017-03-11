#pragma once

#include "FlowControl/Manipulators/Helpers/OrderedCardsManager.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline OrderedCardsManager OrderedCardsManager::FromDeck(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).deck_.cards_);
			}
			inline OrderedCardsManager OrderedCardsManager::FromHand(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).hand_.cards_);
			}
			inline OrderedCardsManager OrderedCardsManager::FromMinions(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).minions_.minions_);
			}

			template <state::CardType TargetCardType>
			inline OrderedCardsManager OrderedCardsManager::FromGraveyard(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.others_);
			}
			template <> inline OrderedCardsManager OrderedCardsManager::FromGraveyard<state::kCardTypeMinion>(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.minions_);
			}
			template <> inline OrderedCardsManager OrderedCardsManager::FromGraveyard<state::kCardTypeSpell>(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
			{
				return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.spells_);
			}

			inline void OrderedCardsManager::Insert(state::CardRef card_ref)
			{
				int pos = state_.mgr.Get(card_ref).GetZonePosition();
				if (pos < 0) throw std::exception("invalid position");
				if (pos > container_.size()) throw std::exception("invalid position");

				auto it = container_.insert(container_.begin() + pos, card_ref);

				++it;
				++pos;

				for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
				{
					Manipulate(state_, flow_context_).Card(*it).ZonePosition().Set(pos);
				}
			}

			inline void OrderedCardsManager::Remove(size_t pos)
			{
				if (pos >= container_.size()) throw std::exception("invalid position");

				auto it = container_.erase(container_.begin() + pos);
				for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
				{
					Manipulate(state_, flow_context_).Card(*it).ZonePosition().Set((int)pos);
				}
			}
		}
	}
}