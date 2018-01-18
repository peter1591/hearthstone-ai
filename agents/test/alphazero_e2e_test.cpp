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
	trainer_config.kTrainingDataCapacityPowerOfTwo = 17; // 131072
	trainer_config.kNeuralNetTrainingBatch = 32;

	// create a random model
	std::string net_path = "neural_net";
	neural_net::NeuralNetwork::CreateWithRandomWeights(net_path);

	neural_net::NeuralNetwork net;
	net.Load(net_path);

	std::string model_path = "";
	trainer.Initialize(trainer_config, net, random);

	trainer.Train();

	trainer.Release();

	return 0;
}