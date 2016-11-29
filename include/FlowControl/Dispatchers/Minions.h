#pragma once

#include "Utils/StaticDispatcher.h"

namespace FlowControl
{
	namespace Dispatchers
	{
		namespace impl
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

			CREATE_INVOKER(BattleCry);

#undef CREATE_INVOKER

			class DefaultInvoked { };
		}

		class Minions
		{
		public:
			using DispatcherImpl = Utils::StaticDispatcher<impl::DefaultInvoked>;

			template <typename... Args>
			static void BattleCry(int id, Args&&... args)
			{
				return DispatcherImpl::Invoke<impl::BattleCryInvoker>(id, std::forward<Args>(args)...);
			}
		};
	}
}

#define REGISTER_MINION_CARD_CLASS(id, ClassName) \
		template <> template <> \
		struct FlowControl::Dispatchers::Minions::DispatcherImpl::DispatchMap<(id)> \
		{ typedef ClassName type; };