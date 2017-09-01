#include <exception>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <random>

#include "json/json.h"

#include "NeuralNetwork.h"

class JsonDataParser : public NeuralNetworkWrapper::IInputGetter
{
public:
	JsonDataParser(Json::Value const& obj) : obj_(obj) {
	}

	double GetField(
		NeuralNetworkWrapper::FieldSide field_side,
		NeuralNetworkWrapper::FieldType field_type,
		int arg1 = 0) override final
	{
		if (field_side == NeuralNetworkWrapper::kCurrent) {
			return GetSideField(field_type, arg1, obj_["current_player"]);
		}
		else if (field_side == NeuralNetworkWrapper::kOpponent) {
			return GetSideField(field_type, arg1, obj_["opponent_player"]);
		}
		throw std::runtime_error("invalid side");
	}

private:
	double GetSideField(NeuralNetworkWrapper::FieldType field_type, int arg1, Json::Value const& side) {
		switch (field_type) {
		case NeuralNetworkWrapper::kResourceCurrent:
		case NeuralNetworkWrapper::kResourceTotal:
		case NeuralNetworkWrapper::kResourceOverload:
		case NeuralNetworkWrapper::kResourceOverloadNext:
			return GetResourceField(field_type, arg1, side["resource"]);

		case NeuralNetworkWrapper::kHeroHP:
		case NeuralNetworkWrapper::kHeroArmor:
			return GetHeroField(field_type, arg1, side["hero"]);

		case NeuralNetworkWrapper::kMinionCount:
		case NeuralNetworkWrapper::kMinionHP:
		case NeuralNetworkWrapper::kMinionMaxHP:
		case NeuralNetworkWrapper::kMinionAttack:
		case NeuralNetworkWrapper::kMinionAttackable:
		case NeuralNetworkWrapper::kMinionTaunt:
		case NeuralNetworkWrapper::kMinionShield:
		case NeuralNetworkWrapper::kMinionStealth:
			return GetMinionsField(field_type, arg1, side["minions"]);

		case NeuralNetworkWrapper::kHandCount:
		case NeuralNetworkWrapper::kHandPlayable:
		case NeuralNetworkWrapper::kHandCost:
			return GetHandField(field_type, arg1, side["hand"]);

		case NeuralNetworkWrapper::kHeroPowerPlayable:
			return GetHeroPowerField(field_type, arg1, side["hero_power"]);

		default:
			throw std::runtime_error("unknown field type");
		}
	}
	
	double GetResourceField(NeuralNetworkWrapper::FieldType field_type, int arg1, Json::Value const& resource) {
		switch (field_type) {
		case NeuralNetworkWrapper::kResourceCurrent:
			return resource["current"].asInt();
		case NeuralNetworkWrapper::kResourceTotal:
			return resource["total"].asInt();
		case NeuralNetworkWrapper::kResourceOverload:
			return resource["overload_current"].asInt();
		case NeuralNetworkWrapper::kResourceOverloadNext:
			return resource["overload_next"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHeroField(NeuralNetworkWrapper::FieldType field_type, int arg1, Json::Value const& hero) {
		switch (field_type) {
		case NeuralNetworkWrapper::kHeroHP:
			return hero["hp"].asInt();
		case NeuralNetworkWrapper::kHeroArmor:
			return hero["armor"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetMinionsField(NeuralNetworkWrapper::FieldType field_type, int minion_idx, Json::Value const& minions) {
		switch (field_type) {
		case NeuralNetworkWrapper::kMinionCount:
			return minions.size();
		case NeuralNetworkWrapper::kMinionHP:
			return minions[minion_idx]["hp"].asInt();
		case NeuralNetworkWrapper::kMinionMaxHP:
			return minions[minion_idx]["max_hp"].asInt();
		case NeuralNetworkWrapper::kMinionAttack:
			return minions[minion_idx]["attack"].asInt();
		case NeuralNetworkWrapper::kMinionAttackable:
			return minions[minion_idx]["attackable"].asBool();
		case NeuralNetworkWrapper::kMinionTaunt:
			return minions[minion_idx]["taunt"].asBool();
		case NeuralNetworkWrapper::kMinionShield:
			return minions[minion_idx]["shield"].asBool();
		case NeuralNetworkWrapper::kMinionStealth:
			return minions[minion_idx]["stealth"].asBool();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHandField(NeuralNetworkWrapper::FieldType field_type, int hand_idx, Json::Value const& hand) {
		switch (field_type) {
		case NeuralNetworkWrapper::kHandCount:
			return hand.size();
		case NeuralNetworkWrapper::kHandPlayable:
			return hand[hand_idx]["playable"].asBool();
		case NeuralNetworkWrapper::kHandCost:
			return hand[hand_idx]["cost"].asInt();
		default:
			throw std::runtime_error("unknown field type");
		}
	}

	double GetHeroPowerField(NeuralNetworkWrapper::FieldType field_type, int arg1, Json::Value const& hero_power) {
		switch (field_type) {
		case NeuralNetworkWrapper::kHeroPowerPlayable:
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
		net_.InitializeTrain();
	}

	void AddJsonFile(std::string const& filename, bool for_validate) {
		Json::Value obj;
		Json::Reader reader;
		std::ifstream fs(filename);
		reader.parse(fs, obj);

		std::string winning_player = GetWinPlayer(obj);
		int seq = 1;
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kMainAction") {
				Json::Value const& board = obj[idx]["board"];
				
				if (board["turn"].asInt() <= 4) continue;

				JsonDataParser board_parser(board);
				int label = IsCurrentPlayerWin(board, winning_player) ? 1 : -1;
				
				net_.AddTrainData(&board_parser, label, for_validate);
				++seq;
			}
		}
	}

	void Train()
	{
		net_.Train();
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

private:
	NeuralNetworkWrapper net_;
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
