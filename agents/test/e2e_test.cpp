
#include <chrono>
#include <iostream>
#include <sstream>

#include "engine/Game-impl.h"
#include "Cards/PreIndexedCards.h"
#include "TestStateBuilder.h"
#include "MCTS/inspector/InteractiveShell.h"

static void Initialize()
{
	std::cout << "Reading json file...";
	if (!Cards::Database::GetInstance().Initialize("cards.json")) assert(false);
	Cards::PreIndexedCards::GetInstance().Initialize();
	std::cout << " Done." << std::endl;
}

struct Config
{
	int threads;
	mcts::Config mcts;
};

static Config global_config;

void Run(Config const& config, agents::MCTSRunner * controller, int secs)
{
	auto & s = std::cout;

	auto start_board_getter = [](std::mt19937 & rand) -> state::State {
		return TestStateBuilder().GetState(rand());
	};

	s << "Running for " << secs << " seconds with " << config.threads << " threads ";

	auto start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point run_until =
		std::chrono::steady_clock::now() +
		std::chrono::seconds(secs);

	long long last_show_rest_sec = -1;
	auto continue_checker = [&]() {
		auto now = std::chrono::steady_clock::now();
		if (now > run_until) return false;

		auto rest_sec = std::chrono::duration_cast<std::chrono::seconds>(run_until - now).count();
		if (rest_sec != last_show_rest_sec) {
			s << "Rest seconds: " << rest_sec << std::endl;
			last_show_rest_sec = rest_sec;
		}
		return true;
	};

	auto start_i = controller->GetStatistic().GetSuccededIterates();
	controller->Run(config.mcts, config.threads, start_board_getter);
	while (true) {
		if (!continue_checker()) break;
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	controller->NotifyStop();
	controller->WaitUntilStopped();
	auto end_i = controller->GetStatistic().GetSuccededIterates();

	s << std::endl;
	s << "Done iterations: " << (end_i - start_i) << std::endl;
	s << "====== Statistics =====" << std::endl;
	controller->GetStatistic().GetDebugMessage();

	auto now = std::chrono::steady_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
	auto speed = (double)(end_i - start_i) / ms * 1000;
	s << "Iterations per second: " << speed << std::endl;
	s << std::endl;
}

bool CheckRun(std::string const& cmdline, agents::MCTSRunner * controller)
{
	std::stringstream ss(cmdline);

	std::string cmd;
	ss >> cmd;

	if (cmd == "t" || cmd == "threads") {
		ss >> global_config.threads;
		return true;
	}

	if (cmd == "s" || cmd == "start") {
		int secs = 0;
		ss >> secs;
		Run(global_config, controller, secs);
		return true;
	}
	return false;
}

void TestAI()
{
	global_config.threads = 1;
	global_config.mcts.SetNeuralNetPath("neural_net_e2e_test");

	srand(0);

	int tree_samples = 10000;
	unsigned int seed = std::random_device()();
	std::mt19937 rand(seed);

	agents::MCTSRunner controller(tree_samples, rand);
	mcts::inspector::InteractiveShell handler(global_config.mcts, &controller);

	while (std::cin) {
		std::string cmdline;
		std::cout << "Command: ";
		std::getline(std::cin, cmdline);

		if (cmdline == "q" || cmdline == "quit") {
			std::cout << "Good bye!" << std::endl;
			break;
		}

		if (CheckRun(cmdline, &controller)) {
			continue;
		}

		std::istringstream iss(cmdline);
		handler.DoCommand(iss, std::cout);
	}
}

int main(int argc, char *argv[])
{
	Initialize();
	TestAI();
	return 0;
}
