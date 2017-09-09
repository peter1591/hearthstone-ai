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