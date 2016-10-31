#pragma once

#include <string>
#include <iostream>

namespace EventManager
{
	namespace Handlers
	{
		class MinionSummonedOnce
		{
		public:
			MinionSummonedOnce(const std::string& s) : s_(s) {}

			void Handle(HandlersContainerController &controller) {
				std::cout << "MinionSummonedOnce called: " << s_ << std::endl;
				controller.Remove();
			}

		private:
			std::string s_;
		};
	}
}