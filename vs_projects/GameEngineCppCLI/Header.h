#pragma once

#include "..\GameEngineCppWrapper\Header.h"

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

			void Reset();
			int Run(int seconds, int threads);
			void Stop();

			void Destroy();

			void UnmanagedOutputMessageCallback(std::string const& msg);

		private:
			::GameEngineCppWrapper::GameEngine* impl_;
			OutputMessageCallback^ output_message_cb_;
			OutputMessageCallbackCpp^ output_message_cb_cpp_;
		};
	}
}