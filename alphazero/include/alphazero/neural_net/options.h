#pragma once

namespace alphazero
{
	namespace neural_net
	{
		struct RunOptions
		{
			RunOptions() :
				epochs_per_run(1000)
			{}

			int epochs_per_run;
		};
	}
}