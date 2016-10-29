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
			MinionSummoned(const std::string& s) : s_(s) {}

			void Handle() {
				std::cout << "MinionSummoned called: " << s_ << std::endl;
			}

		private:
			std::string s_;
		};
	}
}