#pragma once

#include <utility>
#include <unordered_map>
#include "State/EventManager/impl/HandlersContainer.h"

namespace State
{
	namespace EventManager
	{
		namespace impl {
			template <typename CategoryType, typename HandlerType>
			class CategorizedHandlersContainer
			{
			public:
				// Cloneable by copy semantics
				//    Since the STL container and HandlersContainer are with this property
				static const bool CloneableByCopySemantics = true;

				template <typename CategoryType_, typename HandlerType_>
				void PushBack(CategoryType_&& category, HandlerType_&& handler)
				{
					categories_[category].PushBack(std::forward<HandlerType_>(handler));
				}

				template <typename CategoryType_, typename... Args>
				void TriggerAll(CategoryType_&& category, Args&&... args)
				{
					categories_[category].TriggerAll(std::forward<Args>(args)...);
				}

			private:
				std::unordered_map<CategoryType, HandlersContainer<HandlerType>> categories_;
			};
		}
	}
}