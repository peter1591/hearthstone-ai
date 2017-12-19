#pragma once

#include <vector>
#include <random>

#include "state/State.h"
#include "UI/GameEngineLogger.h"

namespace ui
{
	class SampledBoards
	{
	public:
		SampledBoards(GameEngineLogger & logger) : logger_(logger) {}

		template <class StateGetter>
		void Prepare(int samples, std::mt19937 & random, StateGetter && state_getter)
		{
			samples_.clear();

			while (samples > 0) {
				samples_.push_back(state_getter());
				--samples;
			}
		}

		state::State const& GetState(int sample)
		{
			assert(sample >= 0);
			assert(sample < samples_.size());
			return samples_[sample];
		}

	private:
		std::vector<state::State> samples_;
		GameEngineLogger & logger_;
	};
}