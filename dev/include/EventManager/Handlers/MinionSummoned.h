#pragma once

#include <string>
#include <iostream>

namespace EventManager
{
	namespace Handlers
	{
		class MinionSummoned
		{
		public:
			static const bool CloneableByCopySemantics = true;

			MinionSummoned(const std::string& s) : s_(s) {}

			void Handle(HandlersContainerController &controller) {
				std::cout << "MinionSummoned called: " << s_ << std::endl;
			}

		private:
			std::string s_;
		};
	}
}