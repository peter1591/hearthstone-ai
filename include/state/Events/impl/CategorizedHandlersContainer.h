#pragma once

#include <utility>
#include <unordered_map>
#include "state/Events/impl/HandlersContainer.h"
#include "state/Types.h"

namespace state
{
	namespace Events
	{
		namespace impl
		{
			template <typename HandlerType>
			class CategorizedHandlersContainer
			{
			public:
				// Cloneable by copy semantics
				//    Since the STL container and HandlersContainer are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename HandlerType_>
				void PushBack(CardRef card_ref, HandlerType_&& handler)
				{
					categories_[card_ref.id].PushBack(std::forward<HandlerType_>(handler));
				}

				template <typename... Args>
				void TriggerAll(CardRef card_ref, Args&&... args)
				{
					categories_[card_ref.id].TriggerAll(card_ref, std::forward<Args>(args)...);
				}

			private:
				std::unordered_map<state::CardRef::IdentifierType, HandlersContainer<HandlerType>> categories_;
			};
		}
	}
}