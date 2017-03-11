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

				static OrderedCardsManager FromDeck(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);
				static OrderedCardsManager FromHand(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);
				static OrderedCardsManager FromMinions(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);

				template <state::CardType TargetCardType>
				static OrderedCardsManager FromGraveyard(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player);

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
