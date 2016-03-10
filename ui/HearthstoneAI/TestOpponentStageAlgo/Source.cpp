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
	int mulligan_cards; // 3 for the first player; 4 for the second one
	int mulligan_draw_from; // 30

	int hand_cards_mulligan_kept;
	int hand_cards_from_normal_draw;

	struct Card {
		int sequence_id; // must be greater than zero
		bool playable;
		int mulligan_keep_weight; // negative if this card is not present when mulligan (not in initial deck)
	};

	std::vector<Card> hidden_cards; // cards in current deck, and the possible cards in hand
};

class CalcBase {
public:
	virtual int GetNextPlayableCard() = 0;
};

class Simulator : public CalcBase
{
public:
	Simulator(CalcData data)
	{
		this->origin_data = data;
	}

	int GetNextPlayableCard()
	{
		return this->GetNextPlayableCard(this->origin_data);
	}

private:
	int GetNextPlayableCard(CalcData data)
	{
		// deal with mulligan-kept cards
		std::vector<CalcData::Card> hand_cards;
		while (true)
		{
			if (this->DetermineMuliganKeptCards(data, hand_cards) == false) continue;
			break;
		}

		// draw cards to hand

		// numbering cards
		// Card numbering:
		// 0 1 2 3 ................................ 30 (or larger/smaller)
		// <--- hidden cards -->
		//                     <--- played cards --->

		// Remove the hand cards from hidden-cards, since they are already added to hand
		for (auto it = data.hidden_cards.begin(); it != data.hidden_cards.end();)
		{
			bool already_in_hand = false;
			for (auto const& hand_card : hand_cards) {
				if (hand_card.sequence_id == it->sequence_id) {
					already_in_hand = true;
					break;
				}
			}

			// card in hand --> remove from hidden_cards set
			if (already_in_hand) {
				it = data.hidden_cards.erase(it);
			}
			else {
				++it;
			}
		}

		// draw random cards to hand
		if (data.hand_cards_from_normal_draw > data.hidden_cards.size()) throw new std::exception("not enough hidden cards");
		for (int i = 0; i < data.hand_cards_from_normal_draw; ++i) {
			int r = rand() % data.hidden_cards.size();
			hand_cards.push_back(data.hidden_cards[r]);
			data.hidden_cards.erase(data.hidden_cards.begin() + r);
		}

		// discard non-playable-card from hand
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			if (it->playable) ++it;
			else {
				// non-playable-card --> discard it
				it = hand_cards.erase(it);
			}
		}

		// Randomly pick a playable card
		if (hand_cards.empty()) return 0;
		int r = rand() % hand_cards.size();
		return hand_cards[r].sequence_id;
	}

private:

	// return false if mulligan_input is not valid
	bool DetermineMuliganKeptCards(CalcData data, std::vector<CalcData::Card> & hand_cards)
	{
		if (data.hand_cards_mulligan_kept == 0) return true;

		// Card numbering for mulligan deck
		// 0 1 2 3 ...................................................... 30
		// <--- hidden cards in initial deck --->
		//                                      <--- some played cards --->

		std::vector<CalcData::Card> mulligan_deck; // the sequence_id of cards; -1 for played cards
		mulligan_deck.reserve(data.mulligan_draw_from);
		for (auto const& hidden_card : data.hidden_cards) {
			if (hidden_card.mulligan_keep_weight >= 0) {
				mulligan_deck.push_back(hidden_card);
			}
		}
		if (mulligan_deck.size() > data.mulligan_draw_from) throw new std::exception("invalid argument: too many initial cards");

		// fill up played-cards to mulligan_deck
		for (int i = mulligan_deck.size(); i < data.mulligan_draw_from; ++i) {
			CalcData::Card played_card;
			played_card.sequence_id = -1; // indicates a played card
			mulligan_deck.push_back(played_card);
		}

		if (mulligan_deck.size() < data.mulligan_cards) throw new std::exception("invalid argument: too less mulligan deck cards");

		std::vector<CalcData::Card> mulligan_input;
		for (int i = 0; i < data.mulligan_cards; ++i)
		{
			int r = rand() % mulligan_deck.size();

			if (mulligan_deck[r].sequence_id < 0) {
				// this is a played card, so it is impossible to be still in hand
				// --> don't add to mulligan_input
			}
			else if (mulligan_deck[r].mulligan_keep_weight == 0) {
				// this card gets no chance to be kept from mulligan
				// --> don't add to mulligan_input
			}
			else {
				mulligan_input.push_back(mulligan_deck[r]);
			}

			mulligan_deck.erase(mulligan_deck.begin() + r);
		}

		if (data.hand_cards_mulligan_kept > (int)mulligan_input.size()) {
			// Note: If a mulligan-kept card is in hand, but the mulligan input cards are ALL PLAYED
			//       Then, this is an invalid mulligan input since we ACTUALLY have one MULLIGAN-KEPT card in hand which is not played
			//       We need to re-generate the mulligan input
			return false;
		}

		// determine the mulligan-kept cards
		for (int i = 0; i < data.hand_cards_mulligan_kept; ++i)
		{
			int mulligan_input_idx = this->KeepOneMuliganCard(mulligan_input);
			hand_cards.push_back(mulligan_input[mulligan_input_idx]);
			mulligan_input.erase(mulligan_input.begin() + mulligan_input_idx);
		}
		return true;
	}

	// return the index of 'mulligan_input' which is determined to be kept
	size_t KeepOneMuliganCard(std::vector<CalcData::Card> const& mulligan_input)
	{
		if (mulligan_input.empty()) throw new std::exception("invalid input");

		if (mulligan_input.size() == 1) return 0;

		int accumulated_mulligan_weight = 0;

		for (auto const& card : mulligan_input) {
			accumulated_mulligan_weight += card.mulligan_keep_weight;
		}

		unsigned int r = (unsigned int)(rand()) % accumulated_mulligan_weight;

		for (size_t i = 0; i < mulligan_input.size() - 1; ++i) {
			if (r < mulligan_input[i].mulligan_keep_weight) {
				return i;
			}
			r -= mulligan_input[i].mulligan_keep_weight;
		}
		return mulligan_input.size() - 1;
	}

private:
	CalcData origin_data;
};

class Refined : public CalcBase
{
public:
	Refined(CalcData data) : origin_data(data), draw_helper(this->origin_data)
	{
		for (int i = 0; i < data.mulligan_cards; ++i) {
			CalcData::Card invalid_card;
			invalid_card.sequence_id = -1;
			this->mulligan_input.push_back(invalid_card);
		}
	}

	int GetNextPlayableCard()
	{
		return this->GetNextPlayableCard(this->origin_data);
	}

private:
	int GetNextPlayableCard(CalcData data)
	{
		this->draw_helper.Reset();

		// deal with mulligan-kept cards
		hand_cards.clear();
		this->DetermineMuliganKeptCards(data, hand_cards);

		// draw random cards to hand
		if (data.hand_cards_from_normal_draw > data.hidden_cards.size()) throw new std::exception("not enough hidden cards");
		for (int i = 0; i < data.hand_cards_from_normal_draw; ++i) {
			int draw_idx = this->draw_helper.RandomDrawOneHiddenCard();
			hand_cards.push_back(data.hidden_cards[draw_idx]);
		}

		// discard non-playable-card from hand
		for (auto it = hand_cards.begin(); it != hand_cards.end();) {
			if (it->playable) ++it;
			else {
				// non-playable-card --> discard it
				it = hand_cards.erase(it);
			}
		}

		// Randomly pick a playable card
		if (hand_cards.empty()) return 0;
		int r = rand() % hand_cards.size();
		return hand_cards[r].sequence_id;
	}

	// return false if mulligan_input is not valid
	void DetermineMuliganKeptCards(CalcData const& data, std::vector<CalcData::Card> & hand_cards)
	{
		if (data.hand_cards_mulligan_kept == 0) return;

		unsigned int accumulated_mulligan_weight = 0;
		if (mulligan_input.size() != data.mulligan_cards) throw new std::exception("should be allocated in constructor first");
		size_t mulligan_input_size = 0;
		for (int i = 0; i < data.mulligan_cards; ++i)
		{
			int draw_idx = -1;
			if (i < data.hand_cards_mulligan_kept) {
				// these cards are known to be kept from mulligan
				// so, draw them from 'mulligan_keepable_cards'
				draw_idx = this->draw_helper.RandomDrawOneMulliganKeptCard();
			}
			else {
				draw_idx = this->draw_helper.RandomDrawOneMulliganCard();
			}

			if (draw_idx >= 0) {
				CalcData::Card const& card = data.hidden_cards[(unsigned int)draw_idx];

				mulligan_input[mulligan_input_size++] = card;
				accumulated_mulligan_weight += (unsigned int)card.mulligan_keep_weight;
			}
		}

		// determine the mulligan-kept cards
		for (int i = 0; i < data.hand_cards_mulligan_kept; ++i)
		{
			size_t mulligan_input_idx = this->KeepOneMuliganCard(mulligan_input_size, accumulated_mulligan_weight);

			hand_cards.push_back(mulligan_input[mulligan_input_idx]);
			accumulated_mulligan_weight -= mulligan_input[mulligan_input_idx].mulligan_keep_weight;

			mulligan_input_size--;
			std::swap(mulligan_input[mulligan_input_idx], mulligan_input[mulligan_input_size]);
		}
	}

	// return the index of 'mulligan_input' which is determined to be kept
	size_t KeepOneMuliganCard(size_t mulligan_input_size, unsigned int accumulated_mulligan_weight)
	{
		if (mulligan_input_size == 0) throw new std::exception("invalid argument");
		if (mulligan_input_size == 1) return 0;

		unsigned int r = (unsigned int)(rand()) % accumulated_mulligan_weight;

		for (size_t i = 0; i < mulligan_input_size - 1; ++i) {
			if (r < mulligan_input[i].mulligan_keep_weight) {
				return i;
			}
			r -= mulligan_input[i].mulligan_keep_weight;
		}
		return mulligan_input_size - 1;
	}

private:
	class CardDrawHelper
	{
	public:
		CardDrawHelper(CalcData const& data) : data(data)
		{
			// prepare mulligan data
			// the mulligan deck must contains at least 'data.hand_cards_mulligan_kept' mulligan-keepable-cards
			for (size_t i = 0; i < data.hidden_cards.size(); ++i) {
				auto const& hidden_card = data.hidden_cards[i];
				if (hidden_card.mulligan_keep_weight > 0) {
					mulligan_keepable.push_back(i);
				}
				else {
					mulligan_non_keepable_and_not_played.push_back(i);
				}
			}

			if (mulligan_keepable.size() < data.hand_cards_mulligan_kept)
				throw std::exception("not enough cards to be kept from mulligan");
			if (mulligan_keepable.size() + mulligan_non_keepable_and_not_played.size() > data.mulligan_draw_from)
				throw new std::exception("invalid argument: too many initial cards");

			this->Reset();
		}

		void Reset()
		{
			this->mulligan_keepable_size = this->mulligan_keepable.size();
			this->mulligan_non_keepable_and_not_played_size = this->mulligan_non_keepable_and_not_played.size();
			this->mulligan_non_keepable_and_played_size =
				data.mulligan_draw_from - this->mulligan_keepable_size - this->mulligan_non_keepable_and_not_played_size;
		}

		size_t RandomDrawOneMulliganKeptCard()
		{
			// random draw from 'mulligan_keepable'
			unsigned int r = (unsigned int)rand() % (unsigned int)this->mulligan_keepable_size;
			size_t ret = this->mulligan_keepable[r];
			this->RemoveFromMulliganKeepable(r);
			return ret;
		}

		// return >=0 if drawn from hidden_cards
		// return -1 if drawn a played card
		int RandomDrawOneMulliganCard()
		{
			// random draw from 'mulligan_keepable', 'mulligan_non_keepable_and_not_played', and 'mulligan_non_keepable_and_played'
			unsigned int total_outcomes = this->mulligan_keepable_size + this->mulligan_non_keepable_and_not_played_size
				+ this->mulligan_non_keepable_and_played_size;

			unsigned int r = (unsigned int)rand() % total_outcomes;
			
			if (r < this->mulligan_keepable_size) {
				size_t ret = this->mulligan_keepable[r];
				this->RemoveFromMulliganKeepable(r);
				return (int)ret;
			}
			r -= this->mulligan_keepable_size;

			if (r < this->mulligan_non_keepable_and_not_played_size) {
				size_t ret = this->mulligan_non_keepable_and_not_played[r];
				this->RemoveFromMulliganKeepableAndNotPlayed(r);
				return (int)ret;
			}

			this->RemoveFromMulliganKeepableAndPlayed();
			return -1; // draw a played card
		}

		size_t RandomDrawOneHiddenCard()
		{
			// random draw from 'mulligan_keepable' and 'mulligan_non_keepable_and_not_played'
			// random draw from 'mulligan_keepable', 'mulligan_non_keepable_and_not_played', and 'mulligan_non_keepable_and_played'
			unsigned int total_outcomes = this->mulligan_keepable_size + this->mulligan_non_keepable_and_not_played_size;

			unsigned int r = (unsigned int)rand() % total_outcomes;

			if (r < this->mulligan_keepable_size) {
				size_t ret = this->mulligan_keepable[r];
				this->RemoveFromMulliganKeepable(r);
				return ret;
			}
			r -= this->mulligan_keepable_size;

			size_t ret = this->mulligan_non_keepable_and_not_played[r];
			this->RemoveFromMulliganKeepableAndNotPlayed(r);
			return ret;
		}

	private:
		void RemoveFromMulliganKeepable(size_t idx) {
			this->mulligan_keepable_size--;
			std::swap(this->mulligan_keepable[idx], this->mulligan_keepable[this->mulligan_keepable_size]);
		}

		void RemoveFromMulliganKeepableAndNotPlayed(size_t idx) {
			this->mulligan_non_keepable_and_not_played_size--;
			std::swap(this->mulligan_non_keepable_and_not_played[idx],
				this->mulligan_non_keepable_and_not_played[this->mulligan_non_keepable_and_not_played_size]);
		}

		void RemoveFromMulliganKeepableAndPlayed() {
			this->mulligan_non_keepable_and_played_size--;
		}

	private:
		CalcData const& data;

		std::vector<size_t> mulligan_keepable;
		unsigned int mulligan_keepable_size;

		std::vector<size_t> mulligan_non_keepable_and_not_played;
		unsigned int mulligan_non_keepable_and_not_played_size;

		unsigned int mulligan_non_keepable_and_played_size;
	};

private:
	CalcData origin_data;
	CardDrawHelper draw_helper;

	std::vector<CalcData::Card> hand_cards;
	std::vector<CalcData::Card> mulligan_keepable_cards;
	std::vector<CalcData::Card> mulligan_non_keepable_cards;
	std::vector<CalcData::Card> mulligan_input;
};

/*
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
*/

template <typename Calculator, int total_hidden_cards>
class CalcRunner
{
public:
	CalcRunner(std::string const& name, CalcData data) : name(name), runner(data), data(data)
	{
		for (int i = 0; i < total_hidden_cards + 1; i++) {
			this->result[i] = 0;
		}
		this->total_runs = 0;
	}

	void Run()
	{
		++total_runs;
		int next_play_card = runner.GetNextPlayableCard();
		++result[next_play_card];
	}

	void Print()
	{
		std::cout << "Chance for runner '" << this->name << "'" << std::endl;
		for (int i = 0; i<total_hidden_cards + 1; ++i) {
			if (i > 0 && data.hidden_cards[i-1].playable == false) continue;

			std::cout << "\t";
			if (i == 0) {
				std::cout << "No card to play ";
			}
			else {
				std::cout << "Card: " << data.hidden_cards[i-1].sequence_id << " ";
			}
			std::cout << " --> " << (double)(result[i]) / total_runs << std::endl;
		}
	}

private:
	Calculator runner;
	CalcData data;

	std::string name;

	uint64_t total_runs;
	uint64_t result[total_hidden_cards + 1];
};

int main(int argc, char ** argv)
{
	constexpr int total_hidden_cards = 15;

	CalcData data;
	data.mulligan_cards = 3; // 3 for first player; 4 for second player
	data.mulligan_draw_from = 30;
	data.hand_cards_mulligan_kept = 2;
	data.hand_cards_from_normal_draw = 7;

	int sequence_id = 1;
	data.hidden_cards.push_back({ sequence_id++, true, 100 });
	data.hidden_cards.push_back({ sequence_id++, false, 100 });
	data.hidden_cards.push_back({ sequence_id++, true, 50 });
	data.hidden_cards.push_back({ sequence_id++, true, 200 });
	data.hidden_cards.push_back({ sequence_id++, false, 0 });
	data.hidden_cards.push_back({ sequence_id++, false, 0 });
	data.hidden_cards.push_back({ sequence_id++, false, 0 });
	data.hidden_cards.push_back({ sequence_id++, false, 0 });
	data.hidden_cards.push_back({ sequence_id++, false, 0 });
	data.hidden_cards.push_back({ sequence_id++, true, 0 });
	data.hidden_cards.push_back({ sequence_id++, true, 0 });
	while (data.hidden_cards.size() < total_hidden_cards) {
		data.hidden_cards.push_back({ sequence_id++, false, 0 });
	}
	
	int possible_mulligan_kept_cards = 0;
	for (auto const& card : data.hidden_cards) {
		if (card.mulligan_keep_weight > 0) possible_mulligan_kept_cards++;
	}
	if (possible_mulligan_kept_cards < data.hand_cards_mulligan_kept) throw std::runtime_error("not enough card to keep by mulligan.");

	srand((unsigned int)time(NULL));

	auto start = std::chrono::steady_clock::now();

	CalcRunner<Simulator, total_hidden_cards> runner1("Simulator", data);
	CalcRunner<Refined, total_hidden_cards> runner2("Refined", data);

	uint64_t total_runs = 0;
	while (true) {
		runner1.Run();
		runner2.Run();
		total_runs++;

		if (total_runs % 100000 == 0) {
			runner1.Print();
			runner2.Print();

			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
			std::cout << "Done " << total_runs << " iterations in " << elapsed_ms << " ms (Average: " <<
				(total_runs / elapsed_ms * 1000) << " Iterations per second)" << std::endl;
		}
	}

	return 0;
}
