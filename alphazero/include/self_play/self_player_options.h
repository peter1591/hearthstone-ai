#pragma once

namespace alphazero
{
	namespace self_play
	{
		struct SelfPlayerRunOptions
		{
			SelfPlayerRunOptions() :
				MCTS_iterations_per_run(0)
			{}

			int MCTS_iterations_per_run;
		};
	}
}