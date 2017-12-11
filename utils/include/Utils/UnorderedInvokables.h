#pragma once
#include <utility>
#include <functional>
#include <vector>

namespace Utils
{
	template <typename RetType, typename... Args>
	class UnorderedInvokables
	{
	public:
		typedef RetType(*FuncPtrType)(Args...);
		typedef std::function<RetType(Args...)> FunctorType;

		void PushBack(FuncPtrType ptr)
		{
			pointers.push_back(ptr);
		}

		template <typename T, typename = std::enable_if_t<!std::is_convertible<T, FuncPtrType>::value>>
		void PushBack(T&& functor)
		{
			functors.emplace_back(std::forward<T>(functor));
		}

		template <typename... Args>
		void InvokeAll(Args&&... args)
		{
			for (FuncPtrType const& pointer : pointers) pointer(std::forward<Args>(args)...);
			for (FunctorType const& functor : functors) functor(std::forward<Args>(args)...);
		}

	private:
		std::vector<FuncPtrType> pointers;
		std::vector<FunctorType> functors;
	};
}