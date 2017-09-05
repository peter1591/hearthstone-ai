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

		void Initialize() {
			if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
			Cards::PreIndexedCards::GetInstance().Initialize();
		}
		
		void Run(int seconds, int threads) {
			auto run_until = std::chrono::steady_clock::now() +
				std::chrono::seconds(seconds);

			auto continue_checker = [&]() {
				return (run_until > std::chrono::steady_clock::now());
			};

			auto start_board_getter = [](int seed) -> state::State {
				return TestStateBuilder().GetState(seed);
			};

			controller_.Run(continue_checker, threads, start_board_getter);
		}

	private:
		ui::AIController controller_;
	};

	GameEngine::GameEngine() : impl_(nullptr) {
		impl_ = new GameEngineImpl();
	}

	void GameEngine::Initialize() const
	{
		return impl_->Initialize();
	}

	void GameEngine::Run(int seconds, int threads) { return impl_->Run(seconds, threads); }
}