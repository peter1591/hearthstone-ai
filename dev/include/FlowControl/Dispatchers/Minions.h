#pragma once

#include "Utils/StaticDispatcher.h"

namespace FlowControl
{
	namespace Dispatchers
	{
		namespace impl
		{
			template <typename T>
			class BattleCryInvoker
			{
			private:
				typedef long HasMethodType;
				typedef char NoMethodType;

				template <typename U, typename V = decltype(&T::BattleCry)>
				struct SFINAE { using type = HasMethodType; };

				template <typename U> static typename SFINAE<T>::type MethodExists(void*);
				template <typename U> static NoMethodType MethodExists(...);

			public:
				template <typename... Args>
				static void Invoke(Args&&... args)
				{
					using TestResult = decltype(MethodExists<T>(nullptr));
					return InvokeInternal<TestResult>(nullptr, std::forward<Args>(args)...);
				}

			private:
				template <typename TestResult, typename... Args>
				static void InvokeInternal(std::enable_if_t<sizeof(TestResult) == sizeof(HasMethodType)>*, Args&&... args)
				{
					return T::BattleCry(std::forward<Args>(args)...);
				}

				template <typename TestResult, typename... Args>
				static void InvokeInternal(std::enable_if_t<sizeof(TestResult) == sizeof(NoMethodType)>*, Args&&... args)
				{
					return;
				}
			};

			class DefaultInvoked
			{
			public:
				template <typename... Args> static void BattleCry(Args&&... args) {}
			};
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

#define REGISTER_MINION_CARD_CLASS(ClassName) \
		template <> template <> \
		struct FlowControl::Dispatchers::Minions::DispatcherImpl::DispatchMap<ClassName ::id> \
		{ typedef ClassName type; };