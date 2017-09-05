#pragma once

#include "..\GameEngineCppWrapper\Header.h"

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		public ref class GameEngine
		{
		public:
			GameEngine();
			~GameEngine();
			!GameEngine();

			int Initialize();

			int Run(int seconds, int threads);

			void Destroy();

		private:
			::GameEngineCppWrapper::GameEngine* impl_;
		};
	}
}