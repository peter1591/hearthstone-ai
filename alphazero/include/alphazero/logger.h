#pragma once

#include <string>
#include <iostream>

namespace alphazero
{
	class LoggerStream {
	public:
		LoggerStream(
			std::ostream & stream,
			std::string const& prefix,
			std::string const& postfix) :
			stream_(stream),
			prefix_(prefix),
			postfix_(postfix)
		{
			stream_ << prefix_;
		}

		~LoggerStream() {
			stream_ << postfix_;
		}

		template <class T>
		std::ostream & operator<<(T&& msg) {
			return stream_ << std::forward<T>(msg);
		}

	private:
		std::ostream & stream_;
		std::string prefix_;
		std::string postfix_;
	};

	class ILogger
	{
	public:
		virtual ~ILogger() {}

		virtual LoggerStream Info() = 0;
	};

	class StdoutLogger : public ILogger
	{
	public:
		LoggerStream Info() {
			return LoggerStream(
				std::cout,
				"[INFO] ",
				"\n");
		}
	};
}