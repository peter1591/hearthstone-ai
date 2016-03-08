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
		int sequence_id;
		std::string card_id;
		double mulligan_keep_rate; // the rate to keep this card during mulligan
	};
	std::vector<PlayableCard> playable_cards;
};

class CalcBase {
public:
	virtual int GetNextPlayableCard(CalcData data) = 0;
};

class Simulator : public CalcBase
{
public:
	int GetNextPlayableCard(CalcData data)
	{
		hand_cards.clear();

		// random draw for mulligan
		mulligan_deck.clear();
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
		int hand_cards_to_draw = total_hand_cards - (int)hand_cards.size();
		int total_cards = total_hand_cards + data.deck_cards;

		rest_cards.clear();
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
	std::vector<int> hand_cards;
	std::vector<int> mulligan_deck;
	std::vector<int> rest_cards;
};

class Refined : public CalcBase
{
public:
	int GetNextPlayableCard(CalcData data)
	{
		// numbering cards
		// Playable cards: 0 ~ data.playable_cards.size()-1
		int total_hand_cards = data.hand_cards_from_mulligan + data.hand_cards_from_normal_draw;
		int total_cards = total_hand_cards + data.deck_cards;

		if (rest_cards.size() != total_cards) {
			rest_cards.clear();
			for (int i = 0; i < total_cards; ++i) {
				rest_cards.push_back(i);
			}
		}
		else {
			for (int i = 0; i < total_cards; ++i) rest_cards[i] = i;
		}

		// random draw for mulligan
		if (mulligan_deck.size() != data.mulligan_draws_from) {
			mulligan_deck.clear();
			mulligan_deck.reserve(data.mulligan_draws_from);
			for (int i = 0; i < data.mulligan_draws_from; ++i) mulligan_deck.push_back(i);
		}
		else {
			for (int i = 0; i < data.mulligan_draws_from; ++i) mulligan_deck[i] = i;
		}

		// similar to random permutation for the first few elements in 'mulligan_deck'
		hand_cards.clear();
		int hand_cards_size = 0;
		for (int i = 0; i<data.hand_cards_from_mulligan; ++i) {
			int mulligan_deck_size = (data.mulligan_draws_from - i);
			int draw_card_idx = rand() % mulligan_deck_size;

			// swap mulligan_deck[mulligan_deck.size()-1 -i] with mulligan_deck[draw_card_idx]
			// Note: mulligan_deck.size() = data.mulligan_draws_from
			// Note: the value of mulligan_deck[mulligan_deck.size()-1 -i] is not important after swap
			int draw_card = mulligan_deck[draw_card_idx];
			mulligan_deck[draw_card_idx] = mulligan_deck[mulligan_deck_size-1];

			// draw_card is the card got in mulligan input

			// skip non-playable cards
			if (draw_card >= data.playable_cards.size()) continue;

			// did we keep it?
			if (RandomHitProbability(data.playable_cards[draw_card].mulligan_keep_rate)) {
				// card drawn by mulligan should be filtered out
				// --> swap to back, the size will be automatically reduced by 1 since hand_card_size is incremented
				// Note: rest_cards size is: total_cards - hand_cards_size
				rest_cards[draw_card] = rest_cards[total_cards - hand_cards_size - 1];

				hand_cards.push_back(draw_card);
				hand_cards_size++;
			}
		}

		// draw random cards to hand
		// Note: use in-place random permutation algorithm
		// Note: Cards discarded from mulligan are drawn again from deck
		int draw_times = total_hand_cards - hand_cards.size();
		for (int i = 0; i < draw_times; ++i) {
			// size of rest_cards is: total_cards - hand_cards_size
			int rest_card_size = total_cards - hand_cards_size  - i;
			int draw_card_idx = rand() % rest_card_size;

			// swap rest_cards[rest_cards.size()-1 -i] with rest_cards[draw_card_idx]
			// Note: rest_cards.size() = total_cards
			// Note: the value of rest_cards[rest_cards.size()-1 -i] is not important after swap
			int draw_card = rest_cards[draw_card_idx];
			rest_cards[draw_card_idx] = rest_cards[rest_card_size - 1];

			// discard non-playable-card from hand
			// --> so we only add playable-card to hand
			if (draw_card >= data.playable_cards.size()) continue;

			hand_cards.push_back(draw_card);
		}

		// Randomly pick a playable card
		if (hand_cards.empty()) return 0; // no any playable card
		int r = rand() % hand_cards.size();
		return hand_cards[r] + 1;
	}

private:
	std::vector<int> hand_cards;
	std::vector<int> mulligan_deck;
	std::vector<int> rest_cards;
};

class RecursiveMethod : public CalcBase
{
public:
	int GetNextPlayableCard(CalcData data)
	{
		// randomly choose one playable card
		if (data.playable_cards.empty()) 
			return 0;
		int picked_playable_card_idx = rand() % (int)data.playable_cards.size();
		auto picked_playable_card = data.playable_cards.begin() + picked_playable_card_idx;

		// check if the picked playable card is in hand via mulligan
		if (data.hand_cards_from_mulligan > 0) {
			int rand_mulligan = rand() % data.mulligan_draws_from;
			if (rand_mulligan < data.hand_cards_from_mulligan)
			{
				// draw from mulligan, did we keep it?
				if (RandomHitProbability(picked_playable_card->mulligan_keep_rate)) {
					// keep
					return picked_playable_card->sequence_id;
				}
			}
		}
		
		// check if the picked playable card is drawn via normal draws

		// Note: mulligan cards did not get the 'picked_playable_card',
		//       and those cards are normally draw again when dealing mulligan
		int cards_to_draw = data.hand_cards_from_mulligan + data.hand_cards_from_normal_draw;
		int total_cards = cards_to_draw + data.deck_cards;
		int rand_draw = rand() % total_cards;
		if (rand_draw < cards_to_draw) {
			// draw from normal draws
			return picked_playable_card->sequence_id;
		}

		// recursive call
		// Now, the 'picked_playable_card' are out of consideration
		CalcData recursive_data;
		recursive_data.hand_cards_from_mulligan = data.hand_cards_from_mulligan;
		recursive_data.hand_cards_from_normal_draw = data.hand_cards_from_normal_draw;
		recursive_data.deck_cards = data.deck_cards - 1; // now the 'picked_playable_card' are ensured to be in the deck
		recursive_data.mulligan_draws_from = data.mulligan_draws_from - 1;// now we ensure the 'picked_playable_card' will not be drawn from the mulligan

		data.playable_cards.erase(picked_playable_card);
		recursive_data.playable_cards = std::move(data.playable_cards);

		return GetNextPlayableCard(recursive_data);
	}
};

template <int playable_cards>
class CalcRunner
{
public:
	CalcRunner(CalcBase * runner, std::string const& name) : runner(runner), name(name)
	{
		for (int i = 0; i < playable_cards + 1; i++) {
			this->result[i] = 0;
		}
		this->total_runs = 0;
	}

	void Run(CalcData data)
	{
		++total_runs;
		int next_play_card = runner->GetNextPlayableCard(data);
		++result[next_play_card];
	}

	void Print()
	{
		std::cout << "Chance for runner '" << this->name << "'" << std::endl;
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
	}

private:
	CalcBase * runner;
	std::string name;

	uint64_t total_runs;
	uint64_t result[playable_cards + 1];
};

int main(int argc, char ** argv)
{
	constexpr int playable_cards = 3;

	CalcData data;
	data.mulligan_draws_from = 30;
	data.hand_cards_from_mulligan = 2;
	data.hand_cards_from_normal_draw = 2;
	data.deck_cards = 20;
	int sequence_id = 1;
	data.playable_cards.push_back({ sequence_id++, "P1", 1.0 });
	data.playable_cards.push_back({ sequence_id++, "P1", 0.0 });
	data.playable_cards.push_back({ sequence_id++, "P1", 0.5 });

	if (sequence_id-1 != playable_cards) throw std::runtime_error("not match");

	Simulator simulator;
	RecursiveMethod recursive_method;
	Refined refined;

	srand((unsigned int)time(NULL));

	auto start = std::chrono::steady_clock::now();

	CalcRunner<playable_cards> runner1(&simulator, "Simulator");
	CalcRunner<playable_cards> runner2(&recursive_method, "Recursive Method");
	CalcRunner<playable_cards> runner3(&refined, "Refined");

	std::vector<CalcRunner<playable_cards>> runners;
	runners.push_back(runner1);
	//runners.push_back(runner2);
	runners.push_back(runner3);

	uint64_t total_runs = 0;
	while (true) {
		for (auto & runner : runners) runner.Run(data);
		total_runs++;

		if (total_runs % 1000000 == 0) {
			for (auto & runner : runners) runner.Print();

			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
			std::cout << "Done " << total_runs << " iterations in " << elapsed_ms << " ms (Average: " <<
				(total_runs / elapsed_ms * 1000) << " Iterations per second)" << std::endl;
		}
	}

	return 0;
}
