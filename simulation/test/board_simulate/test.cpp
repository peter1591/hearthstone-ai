#include <time.h>

#include <iostream>

#include "card.h"
#include "deck-database.h"
#include "board.h"

#define TIMES_TEST 1000000

double timespec_diff_nsec(struct timespec *start, struct timespec *stop)
{
	double duration = 0;

	duration = stop->tv_sec - start->tv_sec;
	duration += (double)(stop->tv_nsec - start->tv_nsec) / 1000000000;

	return duration;
}

void InitializeDeck1(const DeckDatabase &deck_database, Deck &deck)
{
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
}

void InitializeHand1(const DeckDatabase &deck_database, Hand &hand)
{
	hand.AddCard(deck_database.GetCard(111));
	hand.AddCard(deck_database.GetCard(211));
	hand.AddCard(deck_database.GetCard(213));
	hand.AddCard(deck_database.GetCard(222));
	hand.AddCard(deck_database.GetCard(231));
}

void InitializeBoard(Board &board)
{
	DeckDatabase deck_database;

	InitializeDeck1(deck_database, board.player_deck);
	InitializeHand1(deck_database, board.player_hand);
	
	Minion minion;
	minion.card_id = 111;
	minion.hp = 1;
	minion.max_hp = 1;
	board.player_minions.AddMinion(minion);

	minion.card_id = 213;
	minion.hp = 2;
	minion.max_hp = 3;
	board.player_minions.AddMinion(minion);

	board.PrintBoard();
}

void DoTask(const Board &board)
{
	Board board_new = board;


}

int main(void)
{
	struct timespec start, end;
	Board board;

	InitializeBoard(board);

	while (true) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		std::cout << "Testing board for " << TIMES_TEST << " times... ";
		std::cout.flush();
		for (int i=TIMES_TEST; i>0; --i)
		{
			DoTask(board);
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
