#include <time.h>
#include <iostream>

#include "deck-database.h"
#include "deck.h"

double timespec_diff_nsec(struct timespec *start, struct timespec *stop)
{
	double duration = 0;

	duration = stop->tv_sec - start->tv_sec;
	duration += (double)(stop->tv_nsec - start->tv_nsec) / 1000000000;

	return duration;
}

void PrintDeck(const Deck &deck)
{
	std::cout << "=== Printing Deck ===" << std::endl;
	std::cout << "\tCard: ";
	for (const auto &card : deck.GetCards()) {
		std::cout << card.id << " ";
	}
	std::cout << std::endl;
	std::cout << "=== Printing Deck End ===" << std::endl;
}

void AddCard(const DeckDatabase &deck_database, Deck &deck)
{
	int card_id;

	switch (rand() % 5) {
		case 0:
			card_id = 111;
			break;
		case 1:
			card_id = 211;
			break;
		case 2:
			card_id = 222;
			break;
		case 3:
			card_id = 213;
			break;
		case 4:
			card_id = 231;
			break;
		default:
			abort();
	}

	//std::cout << "Adding a card: " << card_id << std::endl;
	deck.AddCard(deck_database.GetCard(card_id));
//	PrintDeck(deck);
}

void RemoveCard(Deck &deck)
{
	deck.Draw();
//	std::cout << "Removing a card: " << deck.RandomPickAndRemove().id << std::endl;
//	PrintDeck(deck);
}

int main(void)
{
	struct timespec start, end;
	DeckDatabase deck_database;

	srand(time(NULL));

	while (true) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		for (int times=0; times<100000; times++) {
			Deck deck;
			for (int i=0; i<30; i++) {
				AddCard(deck_database, deck);
			}

			for (int i=0; i<60; ++i) {
				if (rand() % 100 == 0) {
					AddCard(deck_database, deck);
				} else {
					RemoveCard(deck);
				}
			}
		}

		if (clock_gettime(CLOCK_MONOTONIC, &end) < 0) {
			std::cerr << "Failed at clock_gettime()" << std::endl;
			return -1;
		}

		std::cout << "Done for " << timespec_diff_nsec(&start, &end) << " secs" << std::endl;
	}

	return 0;
}
