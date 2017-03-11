#pragma once

#include <utility>
#include <vector>
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
					if (card_ref.id >= categories_.size()) {
						categories_.resize(card_ref.id + 1);
					}
					categories_[card_ref.id].PushBack(std::forward<HandlerType_>(handler));
				}

				template <typename... Args>
				void TriggerAll(CardRef card_ref, Args&&... args)
				{
					if (card_ref.id >= categories_.size()) return;
					categories_[card_ref.id].TriggerAll(card_ref, std::forward<Args>(args)...);
				}

			private:
				std::vector<HandlersContainer<HandlerType>> categories_;
			};
		}
	}
}