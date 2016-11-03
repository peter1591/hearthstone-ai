#pragma once

#include <tuple>
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
			typedef std::tuple<int> ArgsTuple;

			MinionSummoned(const std::string& s) : s_(s) {}

			void Handle(HandlersContainerController &controller, int v1, const std::string& s1) {
				std::cout << "MinionSummoned called: " << s_ << ", parameter1 = " << v1
					<< ", parameter2 = " << s1 << std::endl;
			}

		private:
			std::string s_;
		};
	}
}