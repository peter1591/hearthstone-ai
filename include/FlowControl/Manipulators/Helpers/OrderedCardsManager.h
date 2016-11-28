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
				typedef std::vector<state::CardRef> ContainerType;

				OrderedCardsManager(state::State & state, FlowContext & flow_context, ContainerType & container)
					: state_(state), container_(container), flow_context_(flow_context) {}

				static OrderedCardsManager FromDeck(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).deck_.cards_);
				}
				static OrderedCardsManager FromHand(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).hand_.cards_);
				}
				static OrderedCardsManager FromMinions(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).minions_.minions_);
				}

				template <state::CardType TargetCardType>
				static OrderedCardsManager FromGraveyard(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.others_);
				}
				template <> static OrderedCardsManager FromGraveyard<state::kCardTypeMinion>(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.minions_);
				}
				template <> static OrderedCardsManager FromGraveyard<state::kCardTypeSpell>(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
				{
					return OrderedCardsManager(state, flow_context, state.board.Get(player).graveyard_.spells_);
				}

				void Insert(state::CardRef card_ref);
				void Remove(size_t pos);

			private:
				state::State & state_;
				FlowContext & flow_context_;
				ContainerType & container_;
			};
		}
	}
}
