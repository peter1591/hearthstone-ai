#pragma once

#include "UI/GameEngine.h"

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		public ref class GameEngine
		{
		public:
			delegate void OutputMessageCallback(System::String^ msg);
			delegate void OutputMessageCallbackCpp(std::string const& msg);

			GameEngine();
			~GameEngine();
			!GameEngine();

			int Initialize();

			void SetOutputMessageCallback(OutputMessageCallback^ cb);

			int ResetBoard();
			int UpdateBoard(System::String^ board);

			int Run(int seconds, int threads);
			int NotifyStop();

			void Destroy();

			void UnmanagedOutputMessageCallback(std::string const& msg);

		private:
			ui::GameEngine* impl_;
			OutputMessageCallback^ output_message_cb_;
			OutputMessageCallbackCpp^ output_message_cb_cpp_;
		};
	}
}