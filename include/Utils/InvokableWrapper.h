#pragma once
#include <functional>

// If most of the invoked object are pure function pointers, consider to wrap them into this class
// Instaed of wrap them all in a std::function

namespace Utils
{
	template <typename RetType, typename... Args>
	class InvokableWrapper
	{
	public:
		typedef RetType(*FuncPtrType)(Args...);
		typedef std::function<RetType(Args...)> FunctorType;

		InvokableWrapper(FuncPtrType ptr) : is_pointer(true), pointer(ptr) {}

		template <typename T, typename = std::enable_if_t<!std::is_convertible<T, FuncPtrType>::value>>
		InvokableWrapper(T functor) : is_pointer(false), functor(std::move(functor)) {}

		RetType operator()(Args... args) const
		{
			if (is_pointer) return pointer(std::move(args)...);
			else return functor(std::move(args)...);
		}

	private:
		bool is_pointer;
		FuncPtrType pointer;
		FunctorType functor;
	};
}