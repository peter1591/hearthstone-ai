#include <exception>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#pragma warning (push)
#pragma warning (disable: 4083 4244 4267)
#include "tiny_dnn/tiny_dnn.h"
#pragma warning (pop)

#include "json/json.h"

class Trainer
{
public:
	void AddJsonFile(std::string const& filename, bool for_validate) {
		Json::Value obj;
		Json::Reader reader;
		std::ifstream fs(filename);

		//std::cout << "Parsing json file: " << filename << " ...";
		//std::cout.flush();
		reader.parse(fs, obj);
		//std::cout << "Json loaded... ";
		//std::cout.flush();

		std::string winning_player = GetWinPlayer(obj);
		int seq = 1;
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kMainAction") {
				Json::Value const& board = obj[idx]["board"];
				int label = IsCurrentPlayerWin(board, winning_player) ? 1 : -1;
				if (board["turn"].asInt() <= 4) continue;
				AddData(board, label, for_validate);
				//std::cout << seq << " ";
				//std::cout.flush();
				++seq;
			}
		}
		
		//std::cout << "Done." << std::endl;
	}

	void Train()
	{
		static constexpr int hero_in_dim = 1;
		static constexpr int hero_out_dim = 1;

		static constexpr int minion_in_dim = 7;
		static constexpr int minion_middle_dim = 4;
		static constexpr int minion_out_dim = 3;

		static constexpr int minion_count = 7;

		static constexpr int standalone_in_dim = 17;

		tiny_dnn::layers::input in_heroes(tiny_dnn::shape3d(
			hero_in_dim, 1,
			2)); // current hero, opponent hero
		tiny_dnn::layers::conv hero_conv1(
			hero_in_dim, 1,
			hero_in_dim, 1,
			2, 2 * hero_out_dim);
		in_heroes << hero_conv1; // @out: 1 * 1 * (2*hero_out_dim)
		auto hero_conv1a = tiny_dnn::activation::leaky_relu();
		hero_conv1 << hero_conv1a; // @out: 1 * 1 * (2 * hero_out_dim)

		tiny_dnn::layers::input in_minions(tiny_dnn::shape3d(
			minion_in_dim, 1,
			minion_count * 2)); // current minions, opponent minions
		tiny_dnn::layers::conv minion_conv1(
			minion_in_dim, 1,
			minion_in_dim, 1,
			minion_count * 2, minion_count * 2 * minion_middle_dim);
		in_minions << minion_conv1; // @out: 1 * 1 * (minion_count * 2 * minion_middle_dim)
		auto minion_conv1a = tiny_dnn::activation::leaky_relu();
		minion_conv1 << minion_conv1a; // @out: 1 * 1 * (minion_count * 2 * minion_middle_dim)

		tiny_dnn::fully_connected_layer minion_fc1(
			minion_count * 2 * minion_middle_dim,
			minion_count * 2 * minion_out_dim);
		auto minion_conv2a = tiny_dnn::activation::leaky_relu();
		minion_conv1a << minion_fc1 << minion_conv2a;

		tiny_dnn::layers::input in_standalone(tiny_dnn::shape3d(
			1, 1, standalone_in_dim));

		auto concat = tiny_dnn::layers::concat({
			hero_conv1a.out_shape()[0],
			tiny_dnn::shape3d(1,1,minion_count*2*minion_out_dim),
			in_standalone.out_shape()[0]});
		(hero_conv1a, minion_conv2a, in_standalone) << concat;

		auto fc1 = tiny_dnn::fully_connected_layer(
			2 * hero_out_dim + 2 * minion_count * minion_out_dim + standalone_in_dim,
			20);
		concat << fc1;
		
		auto fc1a = tiny_dnn::activation::leaky_relu();
		auto fc2 = tiny_dnn::fully_connected_layer(20, 20);
		auto fc2a = tiny_dnn::activation::leaky_relu();
		auto fc3 = tiny_dnn::fully_connected_layer(20, 1);
		fc1 << fc1a << fc2 << fc2a << fc3;

		tiny_dnn::network<tiny_dnn::graph> net;
		tiny_dnn::construct_graph(net, { &in_heroes, &in_minions, &in_standalone }, { &fc3 });

		size_t batch_size = 32;
		size_t epoch = 10;
		size_t total_epoch = 0;
		tiny_dnn::adam opt;

		while (true) {
			net.fit<tiny_dnn::mse>(opt, input_, output_, batch_size, epoch);

			total_epoch += epoch;

			std::cout << "total epoch: " << total_epoch << std::endl;

			size_t correct = 0;

			for (size_t idx = 0; idx < input_.size(); ++idx) {
				auto result = net.predict(input_[idx]);
				bool predict_win = (result[0][0] > 0.0);
				bool actual_win = (output_[idx][0][0] > 0.0);
				if (predict_win == actual_win) ++correct;
			}
			double rate = ((double)correct) / input_.size();
			std::cout << "test data correct rate: "
				<< rate * 100.0 << "% ("
				<< correct << " / " << input_.size() << ")" << std::endl;

			correct = 0;
			for (size_t idx = 0; idx < validate_input_.size(); ++idx) {
				auto result = net.predict(validate_input_[idx]);
				bool predict_win = (result[0][0] > 0.0);
				bool actual_win = (validate_output_[idx][0][0] > 0.0);
				if (predict_win == actual_win) ++correct;
			}
			rate = ((double)correct) / validate_input_.size();
			std::cout << "validation correct rate: "
				<< rate * 100.0 << "% ("
				<< correct << " / " << validate_input_.size() << ")" << std::endl;
		}
	}

private:
	std::string GetWinPlayer(Json::Value const& obj) {
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kEnd") {
				return obj[idx]["result"].asString();
			}
		}
		throw std::runtime_error("Cannot find win player");
	}

	bool WinPlayerIsFirstPlayer(std::string const& win_player) {
		if (win_player == "kResultFirstPlayerWin") return true;
		if (win_player == "kResultSecondPlayerWin") return false;
		throw std::runtime_error("Failed to parse winning player");
	}

	bool IsCurrentPlayerWin(Json::Value const& board, std::string const& win_player) {
		std::string current_player = board["current_player_id"].asString();
		
		bool current_player_is_first = false;
		if (current_player == "kFirstPlayer") current_player_is_first = true;
		else if (current_player == "kSecondPlayer") current_player_is_first = false;
		else throw std::runtime_error("Failed to parse current player");

		bool win_player_is_first = WinPlayerIsFirstPlayer(win_player);

		return current_player_is_first == win_player_is_first;
	}

	void AddData(Json::Value const& board, int label, bool for_validate) {
		tiny_dnn::tensor_t input;

		tiny_dnn::vec_t input1;
		AddHero(input1, board["current_player"]["hero"]);
		AddHero(input1, board["opponent_player"]["hero"]);
		input.push_back(std::move(input1));

		tiny_dnn::vec_t input2;
		AddMinions(input2, board["current_player"]["minions"]);
		AddMinions(input2, board["opponent_player"]["minions"]);
		input.push_back(std::move(input2));

		tiny_dnn::vec_t input3;
		AddStandAloneData(input3, board);
		input.push_back(std::move(input3));

		if (for_validate) {
			validate_input_.push_back(input);
			validate_output_.push_back({ { (float)label } });
		}
		else {
			input_.push_back(input);
			output_.push_back({ { (float)label } });
		}
	}

	void AddHero(tiny_dnn::vec_t & data, Json::Value const& player) {
		int hp = player["hp"].asInt() + player["armor"].asInt();
		data.push_back(NormalizeFromUniformDist(hp, 0.0, 30.0));
		//data.push_back(player["attack"].asFloat());
		//data.push_back(player["attackable"].asBool());
	}

	void AddMinions(tiny_dnn::vec_t & data, Json::Value const& minions) {
		int rest = 7;
		for (Json::ArrayIndex idx = 0; idx < minions.size(); ++idx) {
			if (rest <= 0) throw std::runtime_error("too many minions");
			AddMinion(data, minions[idx]);
			--rest;
		}
		while (rest > 0) {
			AddMinionPlaceHolder(data);
			--rest;
		}
	}

	void AddMinion(tiny_dnn::vec_t & data, Json::Value const& minion) {
		data.push_back(NormalizeFromUniformDist(minion["hp"].asInt(), 1.0, 7.0));
		data.push_back(NormalizeFromUniformDist(minion["max_hp"].asInt(), 1.0, 7.0));
		data.push_back(NormalizeFromUniformDist(minion["attack"].asFloat(), 0.0, 7.0));
		data.push_back(NormalizeBool(minion["attackable"].asBool()));
		data.push_back(NormalizeBool(minion["taunt"].asBool()));
		data.push_back(NormalizeBool(minion["shield"].asBool()));
		data.push_back(NormalizeBool(minion["stealth"].asBool()));
	}

	void AddMinionPlaceHolder(tiny_dnn::vec_t & data) {
		data.push_back(0.0);
		data.push_back(0.0);
		data.push_back(0.0);
		data.push_back(NormalizeBool(false));
		data.push_back(NormalizeBool(false));
		data.push_back(NormalizeBool(false));
		data.push_back(NormalizeBool(false));
	}

	void AddStandAloneData(tiny_dnn::vec_t & data, Json::Value const& board) {
		data.push_back(NormalizeFromUniformDist(
			board["current_player"]["resource"]["current"].asInt(), 0, 10));
		data.push_back(NormalizeFromUniformDist(
			board["current_player"]["resource"]["total"].asInt(), 0, 10));
		data.push_back(NormalizeFromUniformDist(
			board["current_player"]["resource"]["overload_next"].asInt(), 0, 1));

		int cur_hand_count = board["current_player"]["hand"].size();
		int cur_hand_playable = 0;
		for (Json::ArrayIndex idx = 0; idx < cur_hand_count; ++idx) {
			if (board["current_player"]["hand"][idx]["playable"].asBool()) {
				cur_hand_playable++;
			}
		}

		int hand_cards = 0;
		for (Json::ArrayIndex idx = 0; idx < cur_hand_count; ++idx) {
			data.push_back(NormalizeFromUniformDist(
				board["current_player"]["hand"][idx]["cost"].asInt(), 0, 10));
			++hand_cards;
		}
		while (hand_cards < 10) {
			data.push_back(NormalizeFromUniformDist(-1, 0, 10));
			++hand_cards;
		}

		int opn_hand_count = board["opponent_player"]["hand"].size();

		data.push_back(NormalizeFromUniformDist(cur_hand_count, 0, 10));
		data.push_back(NormalizeFromUniformDist(cur_hand_playable, 0, 10));
		data.push_back(NormalizeFromUniformDist(opn_hand_count, 0, 10));

		data.push_back(NormalizeBool(
			board["current_player"]["hero_power"]["usable"].asBool()));
	}

	double NormalizeFromUniformDist(double v, double min, double max) {
		// normalize to mean = 0, var = 1.0
		// uniform dist is with variance = (max-min)^2 / 12
		// --> so we should have (max-min)^2 / 12 = 1.0
		// --> (max-min)^2 = 12.0
		// --> (max-min) = sqrt(12.0)
		static double sqrt_12 = std::sqrt(12.0);

		double mean = (min + max) / 2;
		double range = (max - min);
		double scale = sqrt_12 / range;

		return (v - mean) * scale;
	}

	double NormalizeBool(bool v) {
		double vv = 0.0;
		if (v) vv = 1.0;
		else vv = -1.0;
		return NormalizeFromUniformDist(vv, -1.0, 1.0);
	}

private:
	std::vector<tiny_dnn::tensor_t> input_;
	std::vector<tiny_dnn::tensor_t> output_;
	std::vector<tiny_dnn::tensor_t> validate_input_;
	std::vector<tiny_dnn::tensor_t> validate_output_;
};

int main(void)
{
	Trainer trainer;

	std::string dirname = "test4";
	std::ifstream filelist(dirname + "/" + "filelist");

	int loaded_files = 0;
	std::random_device rand_dev;
	std::mt19937 rand(rand_dev());

	double validation_case_rate = 0.3; // 30% for validation

	while (filelist) {
		std::string filename;
		filelist >> filename;
		if (filename.empty()) continue;

		std::uniform_real_distribution<double> unif(0.0, 1.0);
		bool for_validate = unif(rand) < validation_case_rate;

		try {
			trainer.AddJsonFile(dirname + "/" + filename, for_validate);
		}
		catch (...) {
			std::cout << "Failed when loading file " << filename << std::endl;
			throw;
		}

		++loaded_files;
		if (loaded_files % 100 == 0) {
			std::cout << "Loaded " << loaded_files << " files" << std::endl;
		}
	}

	trainer.Train();

	return 0;
}
