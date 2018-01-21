#pragma once

namespace alphazero
{
	namespace optimizer
	{
		struct RunOptions
		{
			RunOptions() :
				batch_size(32),
				batches(100),
				epoches(10000),
				epoches_per_run(100),
				maximum_fetch_failure_rate(0.1)
			{}

			int batch_size;
			int batches; // how many batches for training
			int epoches;
			int epoches_per_run;
			double maximum_fetch_failure_rate; // maximum failure rate to fetch training data
		};
	}
}