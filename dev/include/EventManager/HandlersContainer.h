#pragma once

#include <list>
#include <type_traits>
#include <utility>

#include "EventManager/HandlersContainerController.h"

namespace EventManager
{
	template <typename HandlerType>
	class HandlersContainer
	{
	public:
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