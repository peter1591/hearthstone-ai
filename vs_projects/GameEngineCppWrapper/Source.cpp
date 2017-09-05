#include "Header.h"

#include <chrono>

#include "UI/AIController.h"
#include "FlowControl/FlowController-impl.h"
#include "MCTS/TestStateBuilder.h"
#include "Cards/PreIndexedCards.h"

namespace GameEngineCppWrapper
{
	class GameEngineImpl {
	public:
		GameEngineImpl() : controller_() {}

		int Initialize() {
			try {
				if (!Cards::Database::GetInstance().Initialize("cards.json")) return -1;
				Cards::PreIndexedCards::GetInstance().Initialize();
			}
			catch (...) {
				return -1;
			}

			return 0;
		}
		
		int Run(int seconds, int threads) {
			auto run_until = std::chrono::steady_clock::now() +
				std::chrono::seconds(seconds);

			auto continue_checker = [&]() {
				return (run_until > std::chrono::steady_clock::now());
			};

			auto start_board_getter = [](int seed) -> state::State {
				return TestStateBuilder().GetState(seed);
			};

			try {
				controller_.Run(continue_checker, threads, start_board_getter);
			}
			catch (...) {
				return -1;
			}

			return 0;
		}

	private:
		ui::AIController controller_;
	};

	GameEngine::GameEngine() : impl_(nullptr) {
		impl_ = new GameEngineImpl();
	}

	int GameEngine::Initialize() const
	{
		return impl_->Initialize();
	}

	int GameEngine::Run(int seconds, int threads) { return impl_->Run(seconds, threads); }
}