#include <thread>

#include "task.h"

#include "decider.h"

Task::Task(MCTS *mcts)
{
	this->mcts = mcts;
	this->pause_notifier = nullptr;

	// Note: Initialize() might be called after MainLoop() is entered
	this->state = Task::STATE_SPAWNED;
}

Task::~Task()
{
}

Task::Task(Task &&task)
{
	*this = std::move(task);
}

Task & Task::operator=(Task &&task)
{
	this->state = std::move(task.state);
	this->thread = std::move(task.thread);
	this->pause_notifier = std::move(task.pause_notifier);
	this->run_until = std::move(task.run_until);
	this->mcts = std::move(task.mcts);

	return *this;
}

Task::State Task::GetState()
{
	return this->state; // TODO: lock?
}

void Task::SetState(Task::State state)
{
	this->state = state; // TODO: lock?
}

void Task::Initialize(std::thread &&thread)
{
	this->thread = std::move(thread);
}

void Task::Start(std::chrono::time_point<std::chrono::steady_clock> run_until, Task::PauseNotifier *notifier)
{
	this->run_until = run_until;
	this->pause_notifier = notifier;
	this->SetState(Task::STATE_RUNNING);
}

void Task::Stop()
{
	this->SetState(Task::STATE_STOPPED);
}

void Task::ThreadCreatedCallback(Task *task)
{
	task->MainLoop();
}

void Task::MainLoop()
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
				this->pause_notifier->NotifyPaused();
				this->pause_notifier = nullptr;
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
		}
		this->mcts->Iterate();
	}
}

void Task::Join()
{
	this->thread.join();
}