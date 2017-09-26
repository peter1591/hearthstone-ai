
#include <atomic>
#include <chrono>
#include <sstream>
#include <memory>

#include "UI/AIController.h"
#include "UI/BoardGetter.h"
#include "UI/GameEngine.h"
#include "FlowControl/FlowController-impl.h"
#include "Cards/PreIndexedCards.h"

namespace ui
{
	class GameEngineImpl {
	public:
		GameEngineImpl() :
			logger_(),
			running_(false),
			controller_(),
			board_getter_(logger_)
		{}

		int Initialize() {
			try {
				if (!Cards::Database::GetInstance().Initialize("cards.json")) {
					Log("Failed to load cards.json.");
					return -1;
				}
				Cards::PreIndexedCards::GetInstance().Initialize();
			}
			catch (...) {
				Log("Failed to load cards.json.");
				return -1;
			}
			
			Log("Successfully load cards.json.");

			if (ResetBoard() < 0) return -1;

			return 0;
		}

		int ResetBoard() {
			return board_getter_.ResetBoard();
		}

		int UpdateBoard(std::string const& board) {
			return board_getter_.UpdateBoard(board);
		}
		
		int Run(int seconds, int threads) {
			if (running_) {
				Log("Still running.");
				return -1;
			}

			running_ = true;
			int rc = InternalRun(seconds, threads);
			running_ = false;
			return rc;
		}

		int NotifyStop()
		{
			return controller_->NotifyStop();
		}

		void SetOutputMessageCallback(GameEngine::OutputMessageCallback cb)
		{
			logger_.SetOutputMessageCallback(cb);
		}

	private:
		void Log(std::string const& msg)
		{
			logger_.Log(msg);
		}

		int InternalRun(int seconds, int threads)
		{
			auto seed = std::random_device()();

			Log("Start to run.");

			if (!controller_) {
				controller_.reset(new ui::AIController());
			}

			if (board_getter_.PrepareToRun(controller_, seed) < 0) {
				Log("Failed at board_getter_.PrepareToRun().");
				return -1;
			}

			auto run_until = std::chrono::steady_clock::now() +
				std::chrono::seconds(seconds);

			long long last_report_rest_sec = -1;
			auto continue_checker = [&]() {
				auto now = std::chrono::steady_clock::now();
				if (now > run_until) return false;

				auto rest_sec = std::chrono::duration_cast<std::chrono::seconds>(run_until - now).count();
				if (rest_sec != last_report_rest_sec) {
					{
						std::stringstream ss;
						ss << "Rest seconds: " << rest_sec;
						Log(ss.str());
					}

					{
						std::stringstream ss;
						auto iterations = controller_->GetStatistic().GetSuccededIterates();
						ss << "Iterations: " << iterations;
						Log(ss.str());
					}

					last_report_rest_sec = rest_sec;
				}
				return true;
			};

			auto start_board_getter = [this](int seed) -> state::State {
				return board_getter_.GetStartBoard(seed);
			};

			try {
				controller_->Run(continue_checker, threads, seed, start_board_getter);
			}
			catch (...) {
				return -1;
			}

			return 0;
		}

	private:
		GameEngineLogger logger_;
		std::atomic<bool> running_;
		std::unique_ptr<ui::AIController> controller_;
		ui::BoardGetter board_getter_;
	};

	GameEngine::GameEngine() : impl_(nullptr) {
		impl_ = new GameEngineImpl();
	}

	int GameEngine::Initialize() const
	{
		return impl_->Initialize();
	}

	void GameEngine::SetOutputMessageCallback(OutputMessageCallback cb)
	{
		return impl_->SetOutputMessageCallback(cb);
	}

	int GameEngine::ResetBoard() { return impl_->ResetBoard(); }
	int GameEngine::UpdateBoard(std::string const& board) { return impl_->UpdateBoard(board); }
	int GameEngine::Run(int seconds, int threads) { return impl_->Run(seconds, threads); }
	int GameEngine::NotifyStop() { return impl_->NotifyStop(); }
}