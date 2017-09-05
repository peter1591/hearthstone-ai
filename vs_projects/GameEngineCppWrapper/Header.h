#pragma once

namespace GameEngineCppWrapper
{
	class GameEngineImpl;

	class __declspec(dllexport) GameEngine
	{
	public:
		GameEngine();

		int Initialize() const;

		int Run(int seconds, int threads);

	private:
		GameEngineImpl * impl_;
	};
}