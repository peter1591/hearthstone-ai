#pragma once

#include <unordered_set>
#include "Utils/StaticDispatcher.h"

namespace FlowControl
{
	namespace Dispatchers
	{
		namespace Minions_impl
		{
			template <typename T> class ConstructorInvoker
			{
			public:
				static state::Cards::CardData Invoke()
				{
					return T();
				}
			};

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

#undef CREATE_INVOKER

			class DefaultInvoked : public state::Cards::CardData
			{
			public:
				DefaultInvoked()
				{
					throw std::exception("card is not implemented");
				}
			};
		}

		class Minions
		{
		public:
			using DispatcherImpl = Utils::StaticDispatcher<Minions_impl::DefaultInvoked>;

			static state::Cards::CardData CreateInstance(int id)
			{
				return DispatcherImpl::Invoke<Minions_impl::ConstructorInvoker, state::Cards::CardData>(id);
			}
		};
	}
}

#define REGISTER_MINION_CARD_CLASS(ClassName) \
		template <> template <> \
		struct FlowControl::Dispatchers::Minions::DispatcherImpl::DispatchMap<(ClassName::id)> \
		{ typedef ClassName type; };
