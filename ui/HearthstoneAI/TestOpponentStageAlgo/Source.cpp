#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

static bool RandomHitProbability(double prob)
{
	int r = rand();

	// FORMULA: r / (RAND_MAX+1) < prob <--> hit
	// ==> r < prob * (RAND_MAX+1) <--> hit

	int threshold = (int)(prob * (RAND_MAX+1));
	return r < threshold;
}

struct CalcData
{
	int mulligan_draws_from;

	int hand_cards_from_mulligan;
	int hand_cards_from_normal_draw;
	int deck_cards;

	struct PlayableCard {
		std::string card_id;
		double mulligan_keep_rate; // the rate to keep this card during mulligan
	};
	std::vector<PlayableCard> playable_cards;
};

class CalcBase {
public:
	virtual int GetNextPlayableCard() = 0;
};

class Simulator : public CalcBase
{
public:
	Simulator(CalcData data) : data(data) {}

	int GetNextPlayableCard()
	{
		std::vector<int> hand_cards;

		// random draw for mulligan
		std::vector<int> mulligan_deck;
		mulligan_deck.reserve(data.mulligan_draws_from);
		for (int i = 0; i<data.mulligan_draws_from; ++i) mulligan_deck.push_back(i);

		for (int i = 0; i<data.hand_cards_from_mulligan; ++i) {
			int draw_card = rand() % mulligan_deck.size();
			hand_cards.push_back(draw_card);
			mulligan_deck.erase(mulligan_deck.begin() + draw_card);
		}

		// deal mulligan
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			bool keep = false;
			if (*it < (int)data.playable_cards.size()) {
				// got a playable card in mulligan, keep it?
				if (RandomHitProbability(data.playable_cards[*it].mulligan_keep_rate)) {
					keep = true; // keep it
				}
			}

			if (keep) {
				++it;
			}
			else {
				mulligan_deck.push_back(*it);
				it = hand_cards.erase(it);
			}
		}

		// Cards discarded from mulligan are drawn again from deck
		// Treated as a normal draw

		// numbering cards
		// Playable cards: 0 ~ data.playable_cards.size()-1
		int total_hand_cards = data.hand_cards_from_mulligan + data.hand_cards_from_normal_draw;
		int hand_cards_to_draw = total_hand_cards - hand_cards.size();
		int total_cards = total_hand_cards + data.deck_cards;

		std::vector<int> rest_cards;
		for (int i = 0; i<total_cards; ++i) {
			// card drawn by mulligan should be filtered out
			bool filtered_out = false;
			for (auto hand_card : hand_cards) {
				if (i == hand_card) {
					filtered_out = true;
					break;
				}
			}

			if (!filtered_out) rest_cards.push_back(i);
		}

		// draw random cards to hand
		while ((int)hand_cards.size() < total_hand_cards) {
			int r = rand() % rest_cards.size();
			hand_cards.push_back(rest_cards[r]);
			rest_cards.erase(rest_cards.begin() + r);
		}

		// discard non-playable-card from hand
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			if (*it < (int)data.playable_cards.size()) {
				++it;
			}
			else {
				// non-playable-card --> discard it
				it = hand_cards.erase(it);
			}
		}

		// Randomly pick a playable card
		if (hand_cards.empty()) return 0;
		int r = rand() % hand_cards.size();
		return hand_cards[r] + 1;
	}

private:
	CalcData data;
};

class Modified : public CalcBase
{
public:
	Modified(CalcData data) : data(data) {}

	int GetNextPlayableCard()
	{
		std::vector<int> hand_cards;

		// random draw for mulligan
		std::vector<int> mulligan_deck;
		mulligan_deck.reserve(data.mulligan_draws_from);
		for (int i = 0; i<data.mulligan_draws_from; ++i) mulligan_deck.push_back(i);

		for (int i = 0; i<data.hand_cards_from_mulligan; ++i) {
			int draw_card = rand() % mulligan_deck.size();
			hand_cards.push_back(draw_card);
			mulligan_deck.erase(mulligan_deck.begin() + draw_card);
		}

		// deal mulligan
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			bool keep = false;
			if (*it < (int)data.playable_cards.size()) {
				// got a playable card in mulligan, keep it?
				if (RandomHitProbability(data.playable_cards[*it].mulligan_keep_rate)) {
					keep = true; // keep it
				}
			}

			if (keep) {
				++it;
			}
			else {
				mulligan_deck.push_back(*it);
				it = hand_cards.erase(it);
			}
		}

		// Cards discarded from mulligan are drawn again from deck
		// Treated as a normal draw

		// numbering cards
		// Playable cards: 0 ~ data.playable_cards.size()-1
		int total_hand_cards = data.hand_cards_from_mulligan + data.hand_cards_from_normal_draw;
		int hand_cards_to_draw = total_hand_cards - hand_cards.size();
		int total_cards = total_hand_cards + data.deck_cards;

		std::vector<int> rest_cards;
		for (int i = 0; i<total_cards; ++i) {
			// card drawn by mulligan should be filtered out
			bool filtered_out = false;
			for (auto hand_card : hand_cards) {
				if (i == hand_card) {
					filtered_out = true;
					break;
				}
			}

			if (!filtered_out) rest_cards.push_back(i);
		}

		// draw random cards to hand
		while ((int)hand_cards.size() < total_hand_cards) {
			int r = rand() % rest_cards.size();
			hand_cards.push_back(rest_cards[r]);
			rest_cards.erase(rest_cards.begin() + r);
		}

		// discard non-playable-card from hand
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			if (*it < (int)data.playable_cards.size()) {
				++it;
			}
			else {
				// non-playable-card --> discard it
				it = hand_cards.erase(it);
			}
		}

		// Randomly pick a playable card
		if (hand_cards.empty()) return 0;
		int r = rand() % hand_cards.size();
		return hand_cards[r] + 1;
	}

private:
	CalcData data;
};

int main(int argc, char ** argv)
{
	constexpr int playable_cards = 1;

	CalcData data;
	data.mulligan_draws_from = 30;
	data.hand_cards_from_mulligan = 4;
	data.hand_cards_from_normal_draw = 4;
	data.deck_cards = 22;
	data.playable_cards.push_back({ "P1", 1 });

	Simulator simulator(data);
	Modified modified(data);

	srand((unsigned int)time(NULL));

	uint64_t total_runs = 0;

	uint64_t result[playable_cards + 1];
	for (int i = 0; i<playable_cards + 1; ++i) result[i] = 0;

	auto start = std::chrono::steady_clock::now();

	CalcBase * calculator = nullptr;
	if (argc < 2 || std::string(argv[1]) == "simulator")
		//calculator = &simulator;
		calculator = &modified;
	else {
		//calculator = &stage_method;
	}

	while (true) {
		++total_runs;
		int next_play_card = calculator->GetNextPlayableCard();
		++result[next_play_card];

		if (total_runs % 100000 == 0) {
			std::cout << "Chance:" << std::endl;
			for (int i = 0; i<playable_cards + 1; ++i) {
				std::cout << "\t";
				if (i == 0) {
					std::cout << "No card to play ";
				}
				else {
					std::cout << "Card: " << i << " ";
				}
				std::cout << " --> " << (double)(result[i]) / total_runs << std::endl;
			}

			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
			std::cout << "Done " << total_runs << " iterations in " << elapsed_ms << " ms (Average: " <<
				(total_runs / elapsed_ms * 1000) << " Iterations per second)" << std::endl;
		}
	}

	return 0;
}
