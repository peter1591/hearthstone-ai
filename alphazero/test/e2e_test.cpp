#include <random>

#include "alphazero/trainer.h"

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

	std::string model_path = "";
	trainer.Initialize(trainer_config, model_path);

	trainer.Train();

	trainer.Release();

	return 0;
}