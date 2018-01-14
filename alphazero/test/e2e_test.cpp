#include "alphazero/trainer.h"

int main(void)
{
	alphazero::StdoutLogger logger;
	alphazero::EndDeviceTrainer trainer(logger);

	alphazero::TrainerConfigs trainer_config;
	trainer_config.kEvaluationWinRate = 0.55f;
	trainer_config.kTrainingData = 100000; // 100k
	trainer_config.kTrainingDataNotFullyFilledProb = 0.01f; // 1%

	std::string model_path = "";
	trainer.Initialize(trainer_config, model_path);

	trainer.Train();

	trainer.Release();

	return 0;
}