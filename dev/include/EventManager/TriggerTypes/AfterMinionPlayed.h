#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

namespace Entity
{
	class Card;
}

namespace EventManager
{
	namespace TriggerTypes
	{
		class AfterMinionPlayed
		{
		public:
			typedef std::function<void(HandlersContainerController &, const Entity::Card &)> FunctorType;
			typedef std::tuple<const Entity::Card &> ArgsTuple;

			template <typename T,
				typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
				explicit AfterMinionPlayed(T&& functor) : functor_(functor) {}

			void Handle(HandlersContainerController &controller, const Entity::Card & card)
			{
				functor_(controller, card);
			}

		private:
			FunctorType functor_;
		};
	}
}