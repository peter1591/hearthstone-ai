#pragma once
#include <utility>
#include <vector>
#include "Utils/InvokableWrapper.h"

namespace Utils
{
	template <typename RetType, typename... Args>
	class Invokables
	{
	public:
		template <typename T>
		void PushBack(T&& obj)
		{
			return items.push_back(InvokableWrapper<RetType, Args...>(std::forward<T>(obj)));
		}

		template <typename... Args>
		void InvokeAll(Args&&... args)
		{
			for (auto const& item : items) {
				item(std::forward<Args>(args)...);
			}
		}

	private:
		std::vector<InvokableWrapper<RetType, Args...>> items;
	};
}