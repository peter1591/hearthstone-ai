#pragma once

namespace alphazero
{
	namespace optimizer
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