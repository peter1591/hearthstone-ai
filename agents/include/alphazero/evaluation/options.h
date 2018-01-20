#pragma once

namespace alphazero
{
	namespace evaluation
	{
		struct RunOptions
		{
			RunOptions() :
				runs(1000),
				show_interval_ms(1000)
			{}

			int runs;
			int show_interval_ms;
		};
	}
}