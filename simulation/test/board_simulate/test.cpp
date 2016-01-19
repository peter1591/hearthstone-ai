#include <time.h>

#include <iostream>

#include "card.h"
#include "deck-database.h"
#include "board.h"

#define TIMES_TEST 100000
//#define INTERACTIVE

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

	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystals_total = 0;
	board.player_stat.crystals_current = 0;
	board.player_stat.crystals_locked = 0;
	board.player_stat.crystals_locked_next_turn = 0;

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystals_total = 0;
	board.opponent_stat.crystals_current = 0;
	board.opponent_stat.crystals_locked = 0;
	board.opponent_stat.crystals_locked_next_turn = 0;

	board.opponent_deck.Set(30);

	InitializeDeck1(deck_database, board.player_deck);
	InitializeHand1(deck_database, board.player_hand);
	
	Minion minion;
	minion.card_id = 111;
	minion.attack = 1;
	minion.hp = 1;
	minion.max_hp = 1;
	board.player_minions.AddMinion(minion);

	minion.card_id = 213;
	minion.attack = 2;
	minion.hp = 2;
	minion.max_hp = 3;
	board.player_minions.AddMinion(minion);

	board.SetStateToPlayerTurnStart();
}

void DoTask(Board board)
{
	int choose_move = -1;

	while (true) {
		std::vector<Move> next_moves;

#ifdef INTERACTIVE
		std::cout << "!!! Before Go()" << std::endl;
		board.DebugPrint();
#endif
		board.GetNextMoves(next_moves);

		if (next_moves.empty()) break;

#ifdef INTERACTIVE
		std::cout << "!!! Next moves: " << std::endl;
		for (size_t i=0; i<next_moves.size(); ++i) {
			std::cout << "\t";
			std::cout << i << ". " << next_moves[i].GetDebugString() << std::endl;
		}

		choose_move = -1;
		std::cout << "!!! Choose next move: ";
		std::cin >> choose_move;

		if (choose_move < 0 || choose_move >= (int)next_moves.size()) {
			std::cout << "Invalid input, exiting" << std::endl;
			exit(-1);
		}
#else
		choose_move = rand() % next_moves.size();
#endif
		board.ApplyMove(next_moves[choose_move]);
	}
}

int main(void)
{
	struct timespec start, end;
	Board board;

	srand(time(NULL));

	InitializeBoard(board);

	while (true) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		std::cout << "Simulate board for " << TIMES_TEST << " times... ";
		std::cout.flush();
		for (int i=TIMES_TEST; i>0; --i)
		{
			DoTask(board);
#ifdef INTERACTIVE
			return -1;
#endif
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
