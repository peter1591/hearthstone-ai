#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // std::tmpnam
#endif

#include <random>

#include "Cards/PreIndexedCards.h"
#include "alphazero/trainer.h"
#include "neural_net/NeuralNetwork.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
}

int main(void)
{
	Initialize();

	alphazero::StdoutLogger logger;
	auto seed = std::random_device()();
	std::mt19937 random(seed);
	alphazero::EndDeviceTrainer trainer(logger, random);

	alphazero::TrainerConfigs trainer_config;
	trainer_config.kEvaluationWinRate = 0.55f;
	trainer_config.kTrainingDataCapacityPowerOfTwo = 13; // 8192
	
	trainer_config.self_play.save_dir = "./self_play";
	trainer_config.self_play.agent_config.threads = 4;
	trainer_config.self_play.agent_config.tree_samples = 1;
	trainer_config.self_play.agent_config.iterations_per_action = 10000;

	trainer_config.optimizer.batch_size = 32;
	trainer_config.optimizer.batches = 100;
	trainer_config.optimizer.epoches = 1000;
	trainer_config.optimizer.epoches_per_run = trainer_config.optimizer.epoches / 10;

	trainer_config.evaluation.runs = 1000;
	trainer_config.evaluation.agent_config.iterations_per_action = 10000;

	trainer_config.kMinimumTraningData = trainer_config.optimizer.batch_size * 1;
	trainer_config.best_net_path_ = "best_net";
	trainer_config.competitor_net_path_ = "competitor_net";

	// create a random model
	// TODO: only create a random model if best_net does not exist
	neural_net::NeuralNetwork::CreateWithRandomWeights(trainer_config.best_net_path_);

	std::string model_path = "";
	trainer.Initialize(trainer_config, random);

	trainer.Train();

	trainer.Release();

	return 0;
}