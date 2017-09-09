#pragma once

#include <string>

namespace GameEngineCppWrapper
{
	typedef void (__stdcall *OutputMessageCallback)(std::string const&);

	class GameEngineImpl;

	class GameEngineLogger
	{
	public:
		GameEngineLogger() : output_message_callback_(nullptr) {}

		void SetOutputMessageCallback(OutputMessageCallback cb)
		{
			output_message_callback_ = cb;
		}

		void Log(std::string const& msg)
		{
			if (output_message_callback_) {
				output_message_callback_(msg);
			}
		}

	private:
		OutputMessageCallback output_message_callback_;
	};

	class __declspec(dllexport) GameEngine
	{
	public:
		GameEngine();

		int Initialize() const;
		void SetOutputMessageCallback(OutputMessageCallback cb);

		int ResetBoard();
		int UpdateBoard(std::string const& board);

		int Run(int seconds, int threads);
		int NotifyStop();

	private:
		GameEngineImpl * impl_;
	};
}