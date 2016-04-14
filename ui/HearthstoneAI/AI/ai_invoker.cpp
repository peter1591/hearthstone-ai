#include <fstream>
#include <random>
#include <thread>

#include "game-ai/game-ai.h"
#include "game-ai/task.h"

#include "ai_invoker.h"

AIInvoker::AIInvoker()
{
	this->current_job = nullptr;
	this->pending_job = nullptr;
	this->state = STATE_NOT_SPAWNED;
}

bool AIInvoker::Initialize()
{
	std::cerr << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
	{
		std::cerr << "Failed to load card data" << std::endl;
		return false;
	}

	this->abort_flag = false;
	this->main_thread = std::thread(AIInvoker::ThreadCallBack, this);

	return true;
}

void AIInvoker::Cleanup()
{
	this->abort_flag = true;
	this->main_thread.join();
}

void AIInvoker::CreateNewTask(Json::Value game)
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([game](AIInvoker* instance) {
		NewGameJob * new_job = new NewGameJob();
		new_job->game = game;
		instance->DiscardAndSetPendingJob(new_job);
		instance->running = true;
	});
}

void AIInvoker::CancelAllTasks()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([] (AIInvoker* instance) {
		instance->DiscardAndSetPendingJob(nullptr);
		instance->running = false; // just pause
	});
}

void AIInvoker::GenerateCurrentBestMoves()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([] (AIInvoker* instance) {
		instance->GenerateCurrentBestMoves_Internal();
	});
}

void AIInvoker::BoardActionStart(Json::Value game)
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([game](AIInvoker* instance) {
		ActionStartJob * new_job = new ActionStartJob();
		new_job->game = game;
		instance->DiscardAndSetPendingJob(new_job);
		instance->running = true;
	});
}

void AIInvoker::WaitCurrentJobPaused()
{
	for (const auto &task : this->tasks) {
		auto it = pause_notifiers.find(task);
		if (it == pause_notifiers.end()) continue;

		it->second->WaitUntilPaused();
		pause_notifiers.erase(it);
	}
}

void AIInvoker::HandleCurrentJob()
{
	if (!this->current_job) return;

	if (dynamic_cast<NewGameJob*>(this->current_job.get()) != nullptr) {
		return this->HandleJob(dynamic_cast<NewGameJob*>(this->current_job.get()));
	}

	if (dynamic_cast<ActionStartJob*>(this->current_job.get()) != nullptr) {
		return this->HandleJob(dynamic_cast<ActionStartJob*>(this->current_job.get()));
	}

	throw std::runtime_error("unhandled job type");
}

void AIInvoker::HandleJob(NewGameJob * job)
{
	constexpr int sec_each_run = 1;

	this->WaitCurrentJobPaused();

	if (this->mcts.empty()) this->InitializeTasks(job->game);

	if (!this->running) return;

	int total_iterations = 0;
	for (auto const& task : this->tasks) total_iterations += task->GetIterationCount();
	std::cerr << "Done " << total_iterations << " iterations" << std::endl;
	std::cerr.flush();

	// start all threads
	auto run_until = std::chrono::steady_clock::now() +
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
	for (const auto &task : this->tasks)
	{
		if (pause_notifiers.find(task) == pause_notifiers.end()) {
			pause_notifiers[task].reset(new Task::PauseNotifier());
		}
		task->Start(run_until, pause_notifiers[task].get());
	}
}

void AIInvoker::HandleJob(ActionStartJob * job)
{
	std::cerr << "@@@@@@@@@@@@@@@@@@ Got an action start job @@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

	this->WaitCurrentJobPaused();

	if (this->mcts.empty()) {
		this->InitializeTasks(job->game);
		return;
	}

	for (auto & each_mcts : this->mcts)
	{
		// TODO: find board in each thread
		JsonBoardFinder::JsonBoardFinder::UpdateMCTS(*each_mcts, job->game);
	}

	//auto test_job = new NewGameJob();
	//test_job->game = job->game;
	//this->HandleJob(test_job);
	//delete test_job;

	this->current_job.reset(nullptr);

	return;
}

void AIInvoker::StopCurrentJob()
{
	for (const auto &task : this->tasks) {
		task->Stop();
		if (this->pause_notifiers.find(task) != this->pause_notifiers.end()) {
			this->pause_notifiers[task]->WaitUntilPaused();
			this->pause_notifiers.erase(task);
		}
	}

	for (const auto &task : this->tasks) {
		task->Join();
	}

	this->current_job.reset(nullptr);

	for (const auto &task : this->tasks) {
		delete task;
	}
	this->tasks.clear();

	for (auto raw_mcts : this->mcts)
	{
		delete raw_mcts;
	}
	this->mcts.clear();
}

void AIInvoker::GenerateCurrentBestMoves_Internal()
{
	this->WaitCurrentJobPaused();

	if (this->mcts.empty()) return;

	Decider decider;
	for (auto each_mcts : this->mcts) {
		decider.Add(*each_mcts);
	}
	auto best_moves = decider.GetBestMoves();
	best_moves.DebugPrint();
}

void AIInvoker::InitializeTasks(Json::Value const& game)
{
	constexpr int threads = 1; // TODO

	std::mt19937 random_generator((unsigned int)time(nullptr));

	for (int i = 0; i < threads; ++i) {

		std::ofstream last_board_logger("./last_board.json", std::ios_base::out);
		last_board_logger << game.toStyledString();
		last_board_logger.flush();

		std::unique_ptr<BoardInitializer> initializer(new BoardJsonParser(game));
		MCTS * new_mcts = new MCTS();
		new_mcts->Initialize(random_generator(), std::move(initializer));
		this->mcts.push_back(new_mcts);

		Task *new_task = new Task(*new_mcts);
		new_task->Initialize(std::thread(Task::ThreadCreatedCallback, new_task));

		this->tasks.push_back(new_task);
	}

}

void AIInvoker::MainLoop()
{
	this->SetState(STATE_RUNNING);

	while (!this->abort_flag)
	{
		// process pending operations
		this->ProcessPendingOperations();

		Job * new_job = this->GetAndClearPendingJob();
		if (new_job) {
			if (dynamic_cast<NewGameJob*>(new_job)) {
				this->StopCurrentJob();
			}
			this->current_job.reset(new_job);
		}

		this->HandleCurrentJob();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	this->StopCurrentJob();

	this->SetState(STATE_STOPPED);
}

void AIInvoker::ThreadCallBack(AIInvoker * instance)
{
	instance->MainLoop();
}

void AIInvoker::ProcessPendingOperations()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	if (this->pending_operations.empty()) return;
	this->pending_operations.front()(this);
	this->pending_operations.erase(this->pending_operations.begin());
}

void AIInvoker::SetState(State state)
{
	std::unique_lock<std::mutex> lock(this->mtx_state);
	this->state = state;
}

AIInvoker::State AIInvoker::GetState()
{
	std::unique_lock<std::mutex> lock(this->mtx_state);
	return this->state;
}

void AIInvoker::DiscardAndSetPendingJob(Job * new_job)
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_job);
	if (this->pending_job) delete this->pending_job;
	this->pending_job = new_job;
}

AIInvoker::Job * AIInvoker::GetAndClearPendingJob()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_job);
	auto job = this->pending_job;
	this->pending_job = nullptr;
	return job;
}