#pragma once

namespace GameEngineCppWrapper
{
	class GameEngineImpl;

	class __declspec(dllexport) GameEngine
	{
	public:
		GameEngine();

		void Initialize() const;

		int Get() const { return 1234; }

		void Run(int seconds, int threads);

	private:
		GameEngineImpl * impl_;
	};
}