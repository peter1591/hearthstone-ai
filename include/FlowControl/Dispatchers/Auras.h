#pragma once

#include <unordered_set>
#include "Utils/StaticDispatcher.h"

namespace FlowControl
{
	namespace Dispatchers
	{
		namespace Auras_impl
		{
#define CREATE_INVOKER(METHOD_NAME) \
			template <typename T> class METHOD_NAME ## Invoker \
			{ \
			private: \
				typedef long HasMethodType; \
				typedef char NoMethodType; \
				template <typename U, typename V = decltype(&T::METHOD_NAME)> struct SFINAE { using type = HasMethodType; }; \
				template <typename U> static typename SFINAE<T>::type MethodExists(void*); \
				template <typename U> static NoMethodType MethodExists(...); \
\
			public: \
				template <typename... Args> static void Invoke(Args&&... args) { \
					using TestResult = decltype(MethodExists<T>(nullptr)); \
					return InvokeInternal<TestResult>(nullptr, std::forward<Args>(args)...); \
				} \
\
			private: \
				template <typename TestResult, typename... Args> \
				static void InvokeInternal(std::enable_if_t<sizeof(TestResult) == sizeof(HasMethodType)>*, Args&&... args) \
				{ \
					return T::METHOD_NAME(std::forward<Args>(args)...); \
				} \
\
				template <typename TestResult, typename... Args> \
				static void InvokeInternal(std::enable_if_t<sizeof(TestResult) == sizeof(NoMethodType)>*, Args&&... args) \
				{ \
					return; \
				} \
			};

			CREATE_INVOKER(GetEligibles);
			CREATE_INVOKER(ApplyOn);
			CREATE_INVOKER(RemoveFrom);

#undef CREATE_INVOKER

			class DefaultInvoked { };
		}

		class Auras
		{
		public:
			using DispatcherImpl = Utils::StaticDispatcher<Auras_impl::DefaultInvoked>;
			template <typename... Args>
			static void GetEligibles(int id, Args&&... args)
			{
				return DispatcherImpl::Invoke<Auras_impl::GetEligiblesInvoker>(id, std::forward<Args>(args)...);
			}

			template <typename... Args>
			static void ApplyOn(int id, Args&&... args)
			{
				return DispatcherImpl::Invoke<Auras_impl::ApplyOnInvoker>(id, std::forward<Args>(args)...);
			}

			template <typename... Args>
			static void RemoveFrom(int id, Args&&... args)
			{
				return DispatcherImpl::Invoke<Auras_impl::RemoveFromInvoker>(id, std::forward<Args>(args)...);
			}
		};
	}
}

#define REGISTER_AURA_CLASS(id, ClassName) \
		template <> template <> \
		struct FlowControl::Dispatchers::Auras::DispatcherImpl::DispatchMap<(id)> \
		{ typedef ClassName type; };