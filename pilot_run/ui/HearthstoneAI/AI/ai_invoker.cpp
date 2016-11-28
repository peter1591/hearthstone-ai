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
	this->last_report_iteration_count = std::chrono::steady_clock::now();
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

void AIInvoker::CreateNewTask(Json::Value const& game)
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

void AIInvoker::BoardActionStart(Json::Value const& game)
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([game](AIInvoker* instance) {
		ActionStartJob * new_job = new ActionStartJob();
		new_job->game = game;
		instance->DiscardAndSetPendingJob(new_job);
		instance->running = true;
	});
}

void AIInvoker::WaitCurrentJob()
{
	for (const auto &task : this->tasks) {
		auto it = task_done_notifiers.find(task);
		if (it == task_done_notifiers.end()) continue;

		it->second->WaitUntilNotified();
		task_done_notifiers.erase(it);
	}
}

void AIInvoker::HandleCurrentJob()
{
	if (!this->current_job) return;
	this->WaitCurrentJob();

	if (dynamic_cast<NewGameJob*>(this->current_job.get()) != nullptr) {
		return this->HandleJob(dynamic_cast<NewGameJob*>(this->current_job.get()));
	}

	throw std::runtime_error("unhandled job type");
}

void AIInvoker::HandleJob(NewGameJob * job)
{
	constexpr int millis_each_run = 100;

	if (!this->running) return;

	if (this->last_report_iteration_count + std::chrono::seconds(1) < std::chrono::steady_clock::now()) {
		int total_iterations = 0;
		for (auto const& task : this->tasks) total_iterations += task->GetIterationCount();
		std::cerr << "Done " << total_iterations << " iterations" << std::endl;
		std::cerr.flush();

		this->last_report_iteration_count = std::chrono::steady_clock::now();
	}

	// start all threads
	auto run_until = std::chrono::steady_clock::now() + std::chrono::milliseconds(millis_each_run);
	for (const auto &task : this->tasks)
	{
		if (this->task_done_notifiers.find(task) == task_done_notifiers.end()) {
			task_done_notifiers[task].reset(new Task::Notifier());
		}
		task->Start(run_until, task_done_notifiers[task].get());
	}
}

void AIInvoker::StopCurrentJob()
{
	for (const auto &task : this->tasks) {
		task->Stop();
		if (this->task_done_notifiers.find(task) != this->task_done_notifiers.end()) {
			this->task_done_notifiers[task]->WaitUntilNotified();
			this->task_done_notifiers.erase(task);
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

void AIInvoker::HandleNewJob(Job * job)
{
	auto new_game_job = dynamic_cast<NewGameJob*>(job);
	if (new_game_job) {
		this->HandleNewJob(new_game_job);
		return;
	}

	auto action_start_job = dynamic_cast<ActionStartJob*>(job);
	if (action_start_job != nullptr) {
		return this->HandleNewJob(action_start_job);
	}

	throw std::runtime_error("cannot handle new job");
}

void AIInvoker::HandleNewJob(NewGameJob * job)
{
	if (this->mcts.empty()) {
		this->InitializeTasks(job->game);
	}
	else {
		throw std::runtime_error("You should not call this twice");
	}

	this->current_job.reset(job);
}

void AIInvoker::HandleNewJob(ActionStartJob * job)
{
	if (!this->current_job) {
		// start a new job
		NewGameJob * adding_job = new NewGameJob();
		adding_job->game = job->game;
		return this->HandleNewJob(adding_job);
	}

	if (this->mcts.empty()) {
		throw std::runtime_error("should not be called");
	}

	this->UpdateBoard(job->game);
}

void AIInvoker::UpdateBoard(Json::Value const & game)
{
	std::list<std::unique_ptr<Task::Notifier>> notifiers;
	for (auto const& task : this->tasks) {
		auto notifier = std::make_unique<Task::Notifier>();
		task->UpdateBoard(game, notifier.get());
		notifiers.push_back(std::move(notifier));
	}
	for (auto const& notifier : notifiers) notifier->WaitUntilNotified();
}

void AIInvoker::GenerateCurrentBestMoves_Internal()
{
	this->WaitCurrentJob();

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

	std::mt19937_64 random_generator((unsigned int)time(nullptr));

	for (int i = 0; i < threads; ++i)
	{
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
			this->HandleNewJob(new_job);
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