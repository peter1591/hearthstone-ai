#pragma once

#include <list>
#include <type_traits>
#include <utility>

#include "EventManager/HandlersContainerController.h"

namespace EventManager
{
	template <typename HandlerType,
			  typename std::enable_if<HandlerType::CloneableByCopySemantics, int>::type = 0>
	class HandlersContainer
	{
	public:
		// Cloneable by copy semantics
		//    Since the STL container and the underlying HandlerType are with this property
		static const bool CloneableByCopySemantics = true;

		void PushBack(const HandlerType & handler)
		{
			handlers_.push_back(handler);
		}

		template <typename... Args>
		void TriggerAll(Args&&... args)
		{
			auto it = handlers_.begin();
			while (it != handlers_.end()) {
				HandlersContainerController controller;

				it->Handle(controller, std::forward<Args>(args)...);

				if (controller.IsRemoved()) {
					it = handlers_.erase(it);
				}
				else {
					++it;
				}
			}
		}

	private:
		typedef std::list<HandlerType> container_type;
		container_type handlers_;
	};
}