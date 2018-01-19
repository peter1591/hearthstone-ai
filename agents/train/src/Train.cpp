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
#include "judge/json/Reader.h"

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

		int seq = 1;
		judge::json::Reader board_reader;
		board_reader.Parse(obj, [&](judge::json::NeuralNetInputGetter const& input, int label) {
			if (for_validate) {
				validate_input_.AddData(&input);
				validate_output_.AddData(label);
			}
			else {
				train_input_.AddData(&input);
				train_output_.AddData(label);
			}
		});

		std::string result = GetResult(obj);
		for (Json::ArrayIndex idx = 0; idx < obj.size(); ++idx) {
			if (obj[idx]["type"].asString() == "kMainAction") {
				Json::Value const& board = obj[idx]["board"];

				if (board["turn"].asInt() <= 4) continue;

				judge::json::Reader board_parser(board);
				int label = IsCurrentPlayerWin(board, result) ? 1 : -1;


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
