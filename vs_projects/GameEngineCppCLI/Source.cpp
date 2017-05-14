#include "Header.h"

#include <string>
#include <msclr\marshal_cppstd.h>
#include <Windows.h>

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		void GameEngine::InitializeLibrary(System::String^ path) {
			std::string native_path = msclr::interop::marshal_as<std::string>(path);
			LoadLibrary(native_path.c_str());
		}

		GameEngine::GameEngine() : impl_(new GameEngineCppWrapper::GameEngine()) {}

		GameEngine::~GameEngine() { Destroy(); }
		GameEngine::!GameEngine() { Destroy(); }

		void GameEngine::Destroy() {
			if (!impl_) return;
			delete impl_;
			impl_ = nullptr;
		}

		int GameEngine::Get()
		{
			return impl_->Get();
		}
	}
}