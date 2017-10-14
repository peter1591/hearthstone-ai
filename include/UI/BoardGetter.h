#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "state/Configs.h"
#include "Cards/Database.h"
#include "UI/Board/Parser.h"

namespace ui
{
	class BoardGetter
	{
	public:
		static constexpr int kDefaultRootSampleCount = 100;

		BoardGetter(GameEngineLogger & logger) :
			logger_(logger), parser_(logger), root_sample_count_(kDefaultRootSampleCount)
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
			return 0;
		}

		int PrepareToRun(std::unique_ptr<ui::AIController> & controller, int seed)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);

			std::mt19937 rand(seed);
			parser_.ChangeBoard(board_raw_, rand);
			
			if (!controller || need_restart_ai_) {
				controller.reset(new ui::AIController(root_sample_count_, rand));
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
			return parser_.GetStartBoard(seed);
		}

	private:
		std::shared_mutex lock_;
		GameEngineLogger & logger_;
		board::Parser parser_;
		std::string board_raw_;
		int root_sample_count_;
		bool need_restart_ai_;
	};
}