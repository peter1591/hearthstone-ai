#pragma once

namespace alphazero
{
	namespace neural_net
	{
		struct OptimizerRunOptions
		{
			OptimizerRunOptions() :
				epochs_per_run(0)
			{}

			int epochs_per_run;
		};
	}
}