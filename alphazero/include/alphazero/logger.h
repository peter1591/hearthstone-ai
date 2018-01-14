#pragma once

#include <string>
#include <iostream>

namespace alphazero
{
	class ILogger
	{
	public:
		virtual ~ILogger() {}

		virtual void Info(std::string const& msg) = 0;
	};

	class StdoutLogger : public ILogger
	{
	public:
		void Info(std::string const& msg) {
			std::cout << "[INFO] " << msg << std::endl;
		}
	};
}