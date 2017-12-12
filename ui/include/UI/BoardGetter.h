#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "state/Configs.h"
#include "Cards/Database.h"
#include "UI/Board/Parser.h"
#include "UI/ActionApplyHelper.h"

namespace ui
{
	class BoardGetter
	{
	public:
		static constexpr int kDefaultRootSampleCount = 100;

		BoardGetter(GameEngineLogger & logger) :
			lock_(), logger_(logger), parser_(logger), action_apply_helper_(),
			board_raw_(), root_sample_count_(kDefaultRootSampleCount), need_restart_ai_()
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
			parser_.ChangeBoard(board_raw_, rand);
			
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

		state::State GetStartBoard(int seed)
		{
			std::shared_lock<std::shared_mutex> lock(lock_);

			state::State game_state = parser_.GetStartBoard(seed);
			action_apply_helper_.ApplyChoices(game_state);
			return game_state;
		}

	private:
		std::shared_mutex lock_;
		GameEngineLogger & logger_;
		board::Parser parser_;
		ActionApplyHelper action_apply_helper_;
		std::string board_raw_;
		int root_sample_count_;
		bool need_restart_ai_;
	};
}