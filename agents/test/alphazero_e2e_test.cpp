#include <random>

#include "alphazero/trainer.h"
#include "neural_net/NeuralNetwork.h"

int main(void)
{
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
	neural_net::NeuralNetworkWrapper::CreateWithRandomWeights(net_path);

	neural_net::NeuralNetworkWrapper net;
	net.LoadModel(net_path);

	std::string model_path = "";
	trainer.Initialize(trainer_config, net);

	trainer.Train();

	trainer.Release();

	return 0;
}