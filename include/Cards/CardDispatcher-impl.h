#pragma once

#include "Cards/CardDispatcher.h"

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/FlowController-impl.h"
#include "Cards/Database.h"
#include "Cards/Cards.h"

namespace Cards
{
	namespace detail
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

		template <int id>
		class DefaultInvoked : public Cards::GeneralCardBase<void>
		{
		public:
			DefaultInvoked()
			{
				auto const& data = Cards::Database::GetInstance().Get((Cards::CardId)id);

				switch (data.card_type) {
				case state::kCardTypeMinion:
				case state::kCardTypeWeapon:
				case state::kCardTypeHero:
					GeneralCardBase::Init(data);
					return;
				}
				throw std::runtime_error("card is not implemented");
			}
		};
	}

	state::Cards::CardData CardDispatcher::CreateInstance(CardId id)
	{
		return DispatcherImpl::Invoke<detail::ConstructorInvoker, state::Cards::CardData>((int)id);
	}
}