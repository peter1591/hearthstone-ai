#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "state/Configs.h"
#include "Cards/Database.h"
#include "engine/ActionApplyHelper.h"
#include "engine/view/board_view/Parser.h"
#include "UI/SampledBoards.h"

namespace ui
{
	class BoardGetter
	{
	public:
		static constexpr int kDefaultRootSampleCount = 100;

		BoardGetter(GameEngineLogger & logger) :
			lock_(), logger_(logger), action_apply_helper_(),
			board_raw_(), root_sample_count_(kDefaultRootSampleCount), need_restart_ai_(),
			sampled_boards_(logger)
		{}

		// @note Should be set before running
		void SetRootSampleCount(int v) {
			root_sample_count_ = v;
		}

		int ResetBoard()
		{
			std::lock_guard<std::shared_mutex> lock(lock_);
			board_raw_.clear();
			need_restart_ai_ = true;
			return 0;
		}

		int UpdateBoard(std::string const& board_str)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);
			if (board_raw_ == board_str) return 0;

			logger_.Log("Updating board.");
			board_raw_ = board_str;

			// TODO: reuse MCTS tree
			need_restart_ai_ = true;

			return 0;
		}

		int PrepareToRun(std::unique_ptr<agents::MCTSRunner> & controller, int seed)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);

			std::mt19937 rand(seed);
			
			engine::view::board_view::Parser parser;
			auto state_restorer = parser.Parse(board_raw_);
			sampled_boards_.Prepare(root_sample_count_, rand, [&]() {
				return state_restorer.RestoreState(rand);
			});
			
			if (!controller || need_restart_ai_) {
				controller.reset(new agents::MCTSRunner(root_sample_count_, rand));
				action_apply_helper_.ClearChoices();
			}
			else {
				// TODO: re-use MCTS tree
				return 0;
			}

			return 0;
		}

		state::State GetStartBoard(unsigned int seed)
		{
			std::shared_lock<std::shared_mutex> lock(lock_);
			return sampled_boards_.GetState(seed % root_sample_count_);
		}

	private:
		std::shared_mutex lock_;
		GameEngineLogger & logger_;
		engine::ActionApplyHelper action_apply_helper_;
		std::string board_raw_;
		int root_sample_count_;
		bool need_restart_ai_;
		SampledBoards sampled_boards_;
	};
}