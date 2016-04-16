#pragma once

#include <thread>
#include "task.h"
#include "json-board-finder/json-board-finder.h"

inline Task::Task(MCTS & mcts) : mcts(mcts)
{
	this->done_notifier = nullptr;
	this->iterations = 0;

	// Note: Initialize() might be called after MainLoop() is entered
	this->state = Task::STATE_SPAWNED;
}

inline Task::State Task::GetState()
{
	std::unique_lock<std::mutex> lck(this->mtx_state);
	return this->state;
}

inline void Task::SetState(Task::State state_)
{
	std::unique_lock<std::mutex> lck(this->mtx_state);
	this->state = state_;
}

inline int Task::GetIterationCount()
{
	std::unique_lock<std::mutex> lck(this->mtx_iterations);
	return this->iterations;
}

inline void Task::IncreaseIterationCount()
{
	std::unique_lock<std::mutex> lck(this->mtx_iterations);
	++this->iterations;
}

inline void Task::Initialize(std::thread &&thread_)
{
	this->thread = std::move(thread_);
}

inline void Task::Start(std::chrono::time_point<std::chrono::steady_clock> run_until_, Task::Notifier *notifier)
{
	this->run_until = run_until_;
	this->done_notifier = notifier;
	this->SetState(Task::STATE_RUNNING);
}

inline void Task::UpdateBoard(Json::Value const & json_board, Task::Notifier *notifier)
{
	this->pending_operations.push_back([this, json_board, notifier] {
		this->mcts.UpdateRoot(json_board);
		notifier->Notify();
	});
}

inline void Task::Stop()
{
	this->SetState(Task::STATE_STOPPED);
}

inline void Task::ThreadCreatedCallback(Task *task)
{
	task->MainLoop();
}

inline void Task::MainLoop()
{
	auto last_yield = std::chrono::steady_clock::now();
	while (true)
	{
		if (this->GetState() == Task::STATE_STOPPED) break;

		// process pending operations
		if (!this->pending_operations.empty()) {
			auto const& operation = this->pending_operations.front();
			operation();
			this->pending_operations.pop_front();
			continue;
		}

		if (this->GetState() == Task::STATE_PAUSE) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		if (this->GetState() == Task::STATE_RUNNING) {
			auto now = std::chrono::steady_clock::now();
			if (now > this->run_until)
			{
				this->SetState(Task::STATE_PAUSE);
				if (this->done_notifier != nullptr) {
					// Set the notifier to nullptr first, since after the Notify() is called
					// Other threads might call Start(), which sets the notifier again
					auto saved_notifier = this->done_notifier;
					this->done_notifier = nullptr;
					saved_notifier->Notify();
				}
				continue;
			}

			this->mcts.Iterate();
			this->IncreaseIterationCount();
		}
	}

	if (this->done_notifier != nullptr) {
		this->done_notifier->Notify();
	}
}

inline void Task::Join()
{
	this->thread.join();
}