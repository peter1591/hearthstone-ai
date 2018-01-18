#pragma once

#include <string>
#include <iostream>
#include <mutex>

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
		ILogger() : mutex_() {}
		virtual ~ILogger() {}

		// not thread safe
		virtual LoggerStream Info() = 0;

		// thread safe
		void Info(std::function<void(LoggerStream &)> func) {
			std::lock_guard<std::mutex> guard(mutex_);
			func(Info());
		}

		// thread safe, if not using any not-thread-safe interface
		void Info(std::string const& msg) {
			std::lock_guard<std::mutex> guard(mutex_);
			Info() << msg;
		}


	private:
		std::mutex mutex_;
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