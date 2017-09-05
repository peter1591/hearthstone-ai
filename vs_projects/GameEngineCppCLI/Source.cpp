#include "Header.h"

#include <Windows.h>

namespace GameEngineCppWrapper
{
	namespace CLI
	{
		int GameEngine::Initialize()
		{
			return impl_->Initialize();
		}

		GameEngine::GameEngine() : impl_(new GameEngineCppWrapper::GameEngine()) {}

		GameEngine::~GameEngine() { Destroy(); }
		GameEngine::!GameEngine() { Destroy(); }

		void GameEngine::Destroy() {
			if (!impl_) return;
			delete impl_;
			impl_ = nullptr;
		}

		int GameEngine::Run(int seconds, int threads)
		{
			return impl_->Run(seconds, threads);
		}
	}
}