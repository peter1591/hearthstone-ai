#include <exception>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <random>

#include "json/json.h"

#include "neural_net/NeuralNetwork.h"

class JsonDataParser : public neural_net::IInputGetter
{
public:
	JsonDataParser(Json::Value const& obj) : obj_(obj) {
	}

	double GetField(
		neural_net::FieldSide field_side,
		neural_net::FieldType field_type,
		int arg1 = 0) override final
	{
		if (field_side == neural_net::FieldSide::kCurrent) {
			return GetSideField(field_type, arg1, obj_["current_player"]);
		}
		else if (field_side == neural_net::FieldSide::kOpponent) {
			return GetSideField(field_type, arg1, obj_["opponent_player"]);
		}
		throw std::runtime_error("invalid side");
	}

private:
	double GetSideField(neural_net::FieldType field_type, int arg1, Json::Value const& side) {
		switch (field_type) {
		case neural_net::FieldType::kResourceCurrent:
		case neural_net::FieldType::kResourceTotal:
		case neural_net::FieldType::kResourceOverload:
		case neural_net::FieldType::kResourceOverloadNext:
			return GetResourceField(field_type, arg1, side["resource"]);

		case neural_net::FieldType::kHeroHP:
		case neural_net::FieldType::kHeroArmor:
			return GetHeroField(field_type, arg1, side["hero"]);

		case neural_net::FieldType::kMinionCount:
		case neural_net::FieldType::kMinionHP:
		case neural_net::FieldType::kMinionMaxHP:
		case neural_net::FieldType::kMinionAttack:
		case neural_net::FieldType::kMinionAttackable:
		case neural_net::FieldType::kMinionTaunt:
		case neural_net::FieldType::kMinionShield:
		case neural_net::FieldType::kMinionStealth:
			return GetMinionsField(field_type, arg1, side["minions"]);

		case neural_net::FieldType::kHandCount:
		case neural_net::FieldType::kHandPlayable:
		case neural_net::FieldType::kHandCost:
			return GetHandField(field_type, arg1, side["hand"]);

		case neural_net::FieldType::kHeroPowerPlayable:
			return GetHeroPowerField(field_type, arg1, side["hero_power"]);

		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetResourceField(neural_net::FieldType field_type, int arg1, Json::Value const& resource) {
		switch (field_type) {
		case neural_net::FieldType::kResourceCurrent:
			return resource["current"].asInt();
		case neural_net::FieldType::kResourceTotal:
			return resource["total"].asInt();
		case neural_net::FieldType::kResourceOverload:
			return resource["overload_current"].asInt();
		case neural_net::FieldType::kResourceOverloadNext:
			return resource["overload_next"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHeroField(neural_net::FieldType field_type, int arg1, Json::Value const& hero) {
		switch (field_type) {
		case neural_net::FieldType::kHeroHP:
			return hero["hp"].asInt();
		case neural_net::FieldType::kHeroArmor:
			return hero["armor"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetMinionsField(neural_net::FieldType field_type, int minion_idx, Json::Value const& minions) {
		switch (field_type) {
		case neural_net::FieldType::kMinionCount:
			return minions.size();
		case neural_net::FieldType::kMinionHP:
			return minions[minion_idx]["hp"].asInt();
		case neural_net::FieldType::kMinionMaxHP:
			return minions[minion_idx]["max_hp"].asInt();
		case neural_net::FieldType::kMinionAttack:
			return minions[minion_idx]["attack"].asInt();
		case neural_net::FieldType::kMinionAttackable:
			return minions[minion_idx]["attackable"].asBool();
		case neural_net::FieldType::kMinionTaunt:
			return minions[minion_idx]["taunt"].asBool();
		case neural_net::FieldType::kMinionShield:
			return minions[minion_idx]["shield"].asBool();
		case neural_net::FieldType::kMinionStealth:
			return minions[minion_idx]["stealth"].asBool();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHandField(neural_net::FieldType field_type, int hand_idx, Json::Value const& hand) {
		switch (field_type) {
		case neural_net::FieldType::kHandCount:
			return hand.size();
		case neural_net::FieldType::kHandPlayable:
			return hand[hand_idx]["playable"].asBool();
		case neural_net::FieldType::kHandCost:
			return hand[hand_idx]["cost"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHeroPowerField(neural_net::FieldType field_type, int arg1, Json::Value const& hero_power) {
		switch (field_type) {
		case neural_net::FieldType::kHeroPowerPlayable:
			return hero_power["playable"].asBool();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

private:
	Json::Value const& obj_;
};

class Trainer
{
public:
	Trainer() : net_() {
		neural_net::NeuralNetwork::CreateWithRandomWeights("initial_mode");
		net_.Load("initial_model");
	}

	void AddJsonFile(std::string const& filename, bool for_validate) {
		Json::Value obj;
		Json::Reader reader;
		std::ifstream fs(filename);
		reader.parse(fs, obj);

		std::string result = GetResult(obj);
		int seq = 1;
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kMainAction") {
				Json::Value const& board = obj[idx]["board"];

				if (board["turn"].asInt() <= 4) continue;

				JsonDataParser board_parser(board);
				int label = IsCurrentPlayerWin(board, result) ? 1 : -1;

				if (for_validate) {
					validate_input_.AddData(&board_parser);
					validate_output_.AddData(label);
				}
				else {
					train_input_.AddData(&board_parser);
					train_output_.AddData(label);
				}

				++seq;
			}
		}
	}

	void Train()
	{
		size_t batch_size = 32;
		int epoch = 10;
		size_t total_epoch = 0;

		while (true) {
			net_.Train(train_input_, train_output_, batch_size, epoch);
			total_epoch += epoch;

			std::stringstream ss;
			ss << "net_result_epoch_" << total_epoch;
			net_.Save(ss.str());

			{
				auto train_verify = net_.Verify(train_input_, train_output_);
				double rate = ((double)train_verify.first) / train_verify.second;
				std::cout << "test data correct rate: "
					<< rate * 100.0 << "% ("
					<< train_verify.first << " / " << train_verify.second << ")" << std::endl;
			}

			{
				auto validate_verify = net_.Verify(validate_input_, validate_output_);
				double rate = ((double)validate_verify.first) / validate_verify.second;
				std::cout << "validation data correct rate: "
					<< rate * 100.0 << "% ("
					<< validate_verify.first << " / " << validate_verify.second << ")" << std::endl;
			}
		}
	}

private:
	std::string GetResult(Json::Value const& obj) {
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kEnd") {
				return obj[idx]["result"].asString();
			}
		}
		throw std::runtime_error("Cannot find win player");
	}

	bool IsResultWin(std::string const& win_player) {
		if (win_player == "kResultFirstPlayerWin") return true;
		if (win_player == "kResultSecondPlayerWin") return false;
		if (win_player == "kResultDraw") return false;
		throw std::runtime_error("Failed to parse winning player");
	}

	bool IsCurrentPlayerWin(Json::Value const& board, std::string const& result) {
		std::string current_player = board["current_player_id"].asString();

		bool current_player_is_first = false;
		if (current_player == "kFirstPlayer") current_player_is_first = true;
		else if (current_player == "kSecondPlayer") current_player_is_first = false;
		else throw std::runtime_error("Failed to parse current player");

		// Note: AI is always helping first player
		bool win_player_is_first = IsResultWin(result);

		return current_player_is_first == win_player_is_first;
	}

private:
	neural_net::NeuralNetwork net_;
	neural_net::NeuralNetworkInput train_input_;
	neural_net::NeuralNetworkOutput train_output_;
	neural_net::NeuralNetworkInput validate_input_;
	neural_net::NeuralNetworkOutput validate_output_;
};

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "Usage: (program) (dirname)" << std::endl;
		return -1;
	}

	Trainer trainer;

	std::string dirname = argv[1];
	std::string filelist_path = dirname + "/filelist";

	std::cout << "Reading from dir: " << dirname << std::endl;
	std::cout << "Filelist file: " << filelist_path << std::endl;

	std::ifstream filelist(filelist_path);

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
