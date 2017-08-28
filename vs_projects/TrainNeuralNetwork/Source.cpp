#include <exception>
#include <fstream>
#include <iostream>
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
		static constexpr int hero_weights = hero_in_dim * hero_out_dim;
		static constexpr int hero_bias = 1;

		static constexpr int minion_in_dim = 2;
		static constexpr int minion_out_dim = 1;
		static constexpr int minion_weights = minion_in_dim * minion_out_dim;
		static constexpr int minion_bias = 1;

		static constexpr int minion_count = 7;

		static constexpr int input_size = (hero_in_dim + minion_count * minion_in_dim) * 2;

		static constexpr int layer1_out_dim = (hero_out_dim + minion_count * minion_out_dim) * 2;
		static constexpr int layer1_weights = hero_weights + minion_weights;
		static constexpr int layer1_bias = hero_bias + minion_bias;


		tiny_dnn::layers::input in_heroes(tiny_dnn::shape3d(hero_in_dim, 2, 1)); // current hero, opponent hero
		tiny_dnn::layers::conv hero_conv1(
			hero_in_dim, 1,
			hero_in_dim, 1,
			2, 2 * hero_out_dim);
		in_heroes << hero_conv1; // @out: 1 * 2 * hero_out_dim
		auto hero_conv1a = tiny_dnn::activation::tanh();
		hero_conv1 << hero_conv1a; // @out: 1 * 2 * hero_out_dim

		tiny_dnn::layers::input in_minions(tiny_dnn::shape3d(
			minion_in_dim,
			minion_count * 2,
			1)); // current minions, opponent minions
		tiny_dnn::layers::conv minion_conv1(
			minion_in_dim, 1,
			minion_in_dim, 1,
			minion_count * 2, minion_count * 2 * minion_out_dim);
		in_minions << minion_conv1; // @out: 1 * (minion_count * 2) * minion_out_dim
		auto minion_conv1a = tiny_dnn::activation::tanh();
		minion_conv1 << minion_conv1a; // @out: 1 * (minion_count * 2) * minion_out_dim

		auto concat = tiny_dnn::layers::concat({
			{ tiny_dnn::shape3d(1,1,2 * hero_out_dim) },
			{ tiny_dnn::shape3d(1,1,minion_count * 2 * minion_out_dim) } });
		(hero_conv1a, minion_conv1a) << concat;

		auto fc = tiny_dnn::fully_connected_layer(
			2 * hero_out_dim + 2 * minion_count * minion_out_dim,
			1);
		concat << fc;

		auto act1 = tiny_dnn::activation::tanh();
		fc << act1;

		tiny_dnn::network<tiny_dnn::graph> net;
		tiny_dnn::construct_graph(net, { &in_heroes, &in_minions }, { &act1 });

		size_t batch_size = 8192;
		size_t epoch = 10;
		size_t total_epoch = 0;
		tiny_dnn::adagrad opt;

		while (true) {
			net.fit<tiny_dnn::mse>(opt, input_, output_, batch_size, epoch);

			total_epoch += epoch;

			std::cout << "total epoch: " << total_epoch << std::endl;
			size_t correct = 0;
			for (size_t idx = 0; idx < validate_input_.size(); ++idx) {
				auto result = net.predict(input_[idx]);
				bool predict_win = (result[0][0] > 0.0);
				bool actual_win = (validate_output_[idx][0][0] > 0.0);
				if (predict_win == actual_win) ++correct;
			}
			double rate = ((double)correct) / validate_input_.size();
			std::cout << "correct rate: "
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
		data.push_back(player["hp"].asFloat() + player["armor"].asFloat());
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
		data.push_back(minion["hp"].asFloat());
		//data.push_back(minion["max_hp"].asFloat());
		data.push_back(minion["attack"].asFloat());
		//data.push_back(minion["attackable"].asBool());
		//data.push_back(minion["taunt"].asBool());
		//data.push_back(minion["shield"].asBool());
		//data.push_back(minion["stealth"].asBool());
	}

	void AddMinionPlaceHolder(tiny_dnn::vec_t & data) {
		data.push_back(0.0);
		//data.push_back(0.0);
		data.push_back(0.0);
		//data.push_back(false);
		//data.push_back(false);
		//data.push_back(false);
		//data.push_back(false);
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

	double validation_case_rate = 0.01; // 1% for validation

	while (filelist) {
		std::string filename;
		filelist >> filename;
		if (filename.empty()) continue;

		std::uniform_real_distribution<double> unif(0.0, 1.0);
		bool for_validate = unif(rand) < validation_case_rate;
		trainer.AddJsonFile(dirname + "/" + filename, for_validate);

		++loaded_files;
		if (loaded_files % 100 == 0) {
			std::cout << "Loaded " << loaded_files << " files" << std::endl;
		}
	}

	trainer.Train();

	return 0;
}