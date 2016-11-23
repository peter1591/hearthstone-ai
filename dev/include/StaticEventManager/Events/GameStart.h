#pragma once

#include <iostream>
#include "StaticEventManager/Triggerer.h"

namespace StaticEventManager
{
	namespace Events
	{
		namespace impl
		{
			namespace GameStart
			{
				class Handler1
				{
				public:
					static void Trigger()
					{
						std::cout << "GameStart::Handler1 called" << std::endl;
					}
				};

				class Handler2
				{
				public:
					static void Trigger()
					{
						std::cout << "GameStart::Handler2 called" << std::endl;
					}
				};
			}
		}

		using GameStartEvent = StaticEventManager::Triggerer<
			impl::GameStart::Handler1,
			impl::GameStart::Handler2>;
	}
}