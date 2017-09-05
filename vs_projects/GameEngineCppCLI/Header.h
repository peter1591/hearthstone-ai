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

			static void InitializeLibrary(System::String^ path);

			void Initialize();

			int Get();

			void Run(int seconds, int threads);

			void Destroy();

		private:
			::GameEngineCppWrapper::GameEngine* impl_;
		};
	}
}