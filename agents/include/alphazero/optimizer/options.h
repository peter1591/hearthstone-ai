#pragma once

namespace alphazero
{
	namespace optimizer
	{
		struct RunOptions
		{
			RunOptions() :
				epochs_per_run(10),
				batch_size(32),
				batches(16),
				maximum_fetch_failure_rate(0.1)
			{}

			int epochs_per_run;
			int batch_size;
			int batches; // how many batches for training
			double maximum_fetch_failure_rate; // maximum failure rate to fetch training data
		};
	}
}