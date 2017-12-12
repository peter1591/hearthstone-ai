#pragma once

#include <string>

#include "ui/GameEngineLogger.h"

namespace ui
{
	class GameEngineImpl;

	class __declspec(dllexport) GameEngine
	{
	public:
		using OutputMessageCallback = GameEngineLogger::OutputMessageCallback;

		GameEngine();

		int Initialize(int root_sample_count) const;
		void SetOutputMessageCallback(OutputMessageCallback cb);

		int ResetBoard();
		int UpdateBoard(std::string const& board);

		int InteractiveShell(std::string const& cmd);

		int Run(int seconds, int threads);
		int NotifyStop();
		std::string GetBestChoice();

	private:
		GameEngineImpl * impl_;
	};
}