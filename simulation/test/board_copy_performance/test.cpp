#include <time.h>

#include <iostream>

#include "card.h"
#include "board.h"

#define TIMES_COPY 10000000

double timespec_diff_nsec(struct timespec *start, struct timespec *stop)
{
	double duration = 0;

	duration = stop->tv_sec - start->tv_sec;
	duration += (double)(stop->tv_nsec - start->tv_nsec) / 1000000000;

	return duration;
}


int main(void)
{
	struct timespec start, end, duration;
	Board board1;
	Board board2;

	board1.player.AddCardToDeck(Card(111));

	while (true) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		std::cout << "Copying board for " << TIMES_COPY << " times... ";
		std::cout.flush();
		for (int i=TIMES_COPY; i>0; --i)
		{
			board2 = board1;
		}

		if (clock_gettime(CLOCK_MONOTONIC, &end) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		std::cout << timespec_diff_nsec(&start, &end) << " secs" << std::endl;
		std::cout.flush();
	}

	return 0;
}
