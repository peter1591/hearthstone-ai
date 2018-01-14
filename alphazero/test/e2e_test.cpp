#include "trainer.h"

int main(void)
{
	alphazero::StdoutLogger logger;
	alphazero::EndDeviceTrainer trainer(logger);

	alphazero::TrainerConfigs trainer_config;
	trainer_config.kEvaluationWinRate = 0.55f;

	std::string model_path = "";
	trainer.Initialize(model_path);
	trainer.SetConfigs(trainer_config);

	trainer.Train();

	trainer.Release();

	return 0;
}