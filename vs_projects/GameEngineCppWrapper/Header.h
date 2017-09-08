#pragma once

#include <string>

namespace GameEngineCppWrapper
{
	typedef void (__stdcall *OutputMessageCallback)(std::string const&);

	class GameEngineImpl;

	class __declspec(dllexport) GameEngine
	{
	public:
		GameEngine();

		int Initialize() const;
		void SetOutputMessageCallback(OutputMessageCallback cb);

		int ResetBoard();
		int UpdateBoard(std::string const& board);

		int Run(int seconds, int threads);
		void Stop();

	private:
		GameEngineImpl * impl_;
	};
}