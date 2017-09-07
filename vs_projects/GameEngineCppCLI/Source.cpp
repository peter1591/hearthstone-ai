#include "Header.h"

#include <string>
#include <msclr\marshal_cppstd.h>

#include <Windows.h>

static void CppOutputMessageCallback(std::string const& msg)
{

}

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		int GameEngine::Initialize()
		{
			output_message_cb_cpp_ = gcnew OutputMessageCallbackCpp(
				this, &GameEngine::UnmanagedOutputMessageCallback);
			System::IntPtr cb_ptr =
				System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(output_message_cb_cpp_);
			impl_->SetOutputMessageCallback(
				static_cast<GameEngineCppWrapper::OutputMessageCallback>(cb_ptr.ToPointer()));

			return impl_->Initialize();
		}

		void GameEngine::SetOutputMessageCallback(OutputMessageCallback ^ cb)
		{
			output_message_cb_ = cb;
		}

		void GameEngine::UnmanagedOutputMessageCallback(std::string const & msg)
		{
			output_message_cb_(msclr::interop::marshal_as<System::String^>(msg));
		}

		GameEngine::GameEngine() : impl_(new GameEngineCppWrapper::GameEngine()) {}

		GameEngine::~GameEngine() { Destroy(); }
		GameEngine::!GameEngine() { Destroy(); }

		void GameEngine::Destroy() {
			if (!impl_) return;
			delete impl_;
			impl_ = nullptr;
		}

		void GameEngine::Reset()
		{
			return impl_->Reset();
		}

		int GameEngine::Run(int seconds, int threads)
		{
			return impl_->Run(seconds, threads);
		}
	}
}