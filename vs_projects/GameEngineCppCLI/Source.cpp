#include "Header.h"

#include <string>
#include <msclr\marshal_cppstd.h>

#include <Windows.h>

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		int GameEngine::Initialize(int root_sample_count)
		{
			output_message_cb_cpp_ = gcnew OutputMessageCallbackCpp(
				this, &GameEngine::UnmanagedOutputMessageCallback);
			System::IntPtr cb_ptr =
				System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(output_message_cb_cpp_);
			impl_->SetOutputMessageCallback(
				static_cast<ui::GameEngine::OutputMessageCallback>(cb_ptr.ToPointer()));

			return impl_->Initialize(root_sample_count);
		}

		void GameEngine::SetOutputMessageCallback(OutputMessageCallback ^ cb)
		{
			output_message_cb_ = cb;
		}

		void GameEngine::UnmanagedOutputMessageCallback(std::string const & msg)
		{
			output_message_cb_(msclr::interop::marshal_as<System::String^>(msg));
		}

		GameEngine::GameEngine() : impl_(new ui::GameEngine()) {}

		GameEngine::~GameEngine() { Destroy(); }
		GameEngine::!GameEngine() { Destroy(); }

		void GameEngine::Destroy() {
			if (!impl_) return;
			delete impl_;
			impl_ = nullptr;
		}

		int GameEngine::ResetBoard()
		{
			return impl_->ResetBoard();
		}

		int GameEngine::UpdateBoard(System::String^ board)
		{
			return impl_->UpdateBoard(msclr::interop::marshal_as<std::string>(board));
		}

		int GameEngine::Run(int seconds, int threads)
		{
			return impl_->Run(seconds, threads);
		}

		int GameEngine::NotifyStop()
		{
			return impl_->NotifyStop();
		}

		int GameEngine::InteractiveShell(System::String^ cmd)
		{
			return impl_->InteractiveShell(msclr::interop::marshal_as<std::string>(cmd));
		}
	}
}