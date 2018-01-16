#pragma once

namespace alphazero
{
	namespace self_play
	{
		struct RunResult
		{
			int generated_count_;

			RunResult() : generated_count_(0) {}

			RunResult & operator+=(RunResult const& rhs) {
				generated_count_ += rhs.generated_count_;
				return *this;
			}

			void Clear() {
				generated_count_ = 0;
			}
		};
	}
}