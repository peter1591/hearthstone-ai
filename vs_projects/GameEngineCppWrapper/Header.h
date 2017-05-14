#pragma once

namespace GameEngineCppWrapper
{
	class __declspec(dllexport) GameEngine
	{
	public:
		GameEngine() : v_(427549) { }

		int Get() const;

	private:
		int v_;
	};
}