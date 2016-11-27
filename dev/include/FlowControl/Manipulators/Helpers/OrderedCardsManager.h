#pragma once

#include <vector>

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class OrderedCardsManager
			{

			public:
				typedef std::vector<CardRef> ContainerType;

				OrderedCardsManager(state::State & state, ContainerType & container) : state_(state), container_(container) {}

				static OrderedCardsManager FromDeck(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).deck_.cards_);
				}
				static OrderedCardsManager FromHand(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).hand_.cards_);
				}
				static OrderedCardsManager FromMinions(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).minions_.minions_);
				}

				template <state::CardType TargetCardType>
				static OrderedCardsManager FromGraveyard(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).graveyard_.others_);
				}
				template <> static OrderedCardsManager FromGraveyard<state::kCardTypeMinion>(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).graveyard_.minions_);
				}
				template <> static OrderedCardsManager FromGraveyard<state::kCardTypeSpell>(state::State & state, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, state.board.Get(player).graveyard_.spells_);
				}

				void Insert(CardRef card_ref);
				void Remove(size_t pos);

			private:
				state::State & state_;
				ContainerType & container_;
			};
		}
	}
}
