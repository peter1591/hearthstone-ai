#pragma once

#include <thread>
#include "task.h"

inline Task::Task(MCTS & mcts) : mcts(mcts)
{
	this->pause_notifier = nullptr;
	this->iterations = 0;

	// Note: Initialize() might be called after MainLoop() is entered
	this->state = Task::STATE_SPAWNED;
}

inline Task::State Task::GetState()
{
	std::unique_lock<std::mutex> lck(this->mtx_state);
	return this->state;
}

inline void Task::SetState(Task::State state)
{
	std::unique_lock<std::mutex> lck(this->mtx_state);
	this->state = state;
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

inline void Task::Initialize(std::thread &&thread)
{
	this->thread = std::move(thread);
}

inline void Task::Start(std::chrono::time_point<std::chrono::steady_clock> run_until, Task::PauseNotifier *notifier)
{
	this->run_until = run_until;
	this->pause_notifier = notifier;
	this->SetState(Task::STATE_RUNNING);
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
		switch (this->GetState())
		{
		case Task::STATE_SPAWNED:
			// spawned but not started yet --> wait
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;

		case Task::STATE_RUNNING:
			break;

		case Task::STATE_PAUSE:
			if (this->pause_notifier != nullptr) {
				// Set the notifier to nullptr first, since after the NotifyPaused() is called
				// Other threads might call Start(), which sets the pause notifier again
				auto saved_notifier = this->pause_notifier;
				this->pause_notifier = nullptr;
				saved_notifier->NotifyPaused();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;

		case Task::STATE_STOPPED:
			return;
		}

		auto now = std::chrono::steady_clock::now();
		if (now > this->run_until)
		{
			this->SetState(Task::STATE_PAUSE);
			continue;
		}

		this->mcts.Iterate();
		this->IncreaseIterationCount();
	}
}

inline void Task::Join()
{
	this->thread.join();
}