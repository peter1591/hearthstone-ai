#pragma once

namespace alphazero
{
	namespace evaluation
	{
		struct EvaluatorRunOptions
		{
			EvaluatorRunOptions() :
				epochs_per_run(0)
			{}

			int epochs_per_run;
		};
	}
}