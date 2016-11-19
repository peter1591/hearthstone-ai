#pragma once

#include "StaticDispatcher/Dispatcher.h"

namespace Cards
{
	namespace Minions
	{
		namespace impl
		{
			template <typename T>
			class BattleCryInvoker
			{
			public:
				template <typename... Args>
				static void Invoke(Args&&... args)
				{
					return T::BattleCry(std::forward<Args>(args)...);
				}
			};

			class DefaultInvoked
			{
			public:
				template <typename... Args> static void BattleCry(Args&&... args) {}
			};
		}

		class Dispatcher
		{
		public:
			using DispatcherImpl = StaticDispatcher::Dispatcher<impl::DefaultInvoked>;

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
		struct Cards::Minions::Dispatcher::DispatcherImpl::DispatchMap<ClassName ::id> \
		{ typedef ClassName type; };