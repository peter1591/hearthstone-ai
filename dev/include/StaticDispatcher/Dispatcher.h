#pragma once

#include <string>
#include <utility>

namespace StaticDispatcher
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

	template <int Id> struct DispatcherMap;

	class Dispatcher
	{
	public:
		template <typename... Args>
		void BattleCry(int id, Args&&... args)
		{
			return Invoke<BattleCryInvoker>(id, std::forward<Args>(args)...);
		}

	private:
		template <template <typename> class InvokerType, typename... Args>
		void Invoke(int id, Args&&... args)
		{
			switch (id) {
			case 0: 
				return InvokerType<DispatcherMap<0>::type>::Invoke(std::forward<Args>(args)...);
			}
		}
	};
}