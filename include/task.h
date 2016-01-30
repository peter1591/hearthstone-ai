#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

#include "mcts.h"

class Task
{
public:
	class PauseNotifier
	{
	public:
		PauseNotifier()
		{
			this->caller_lock = new std::unique_lock<std::mutex>(this->mtx);
		}

		~PauseNotifier()
		{
			delete this->caller_lock;
		}

		PauseNotifier(const PauseNotifier &) = delete;
		PauseNotifier & operator=(const PauseNotifier &) = delete;
		PauseNotifier(const PauseNotifier &&) = delete;
		PauseNotifier & operator=(PauseNotifier &&) = delete;

		void WaitUntilPaused()
		{
			this->cv.wait(*this->caller_lock);
		}

		void NotifyPaused()
		{
			std::unique_lock<std::mutex> notifier_lock(this->mtx); // in case NotifyPaused() is called before WaitUntilPause()
			this->cv.notify_all();
		}

	private:
		std::mutex mtx;
		std::condition_variable cv;
		std::unique_lock<std::mutex> *caller_lock;
	};

public:
	Task(MCTS *mcts);
	~Task();

	static void ThreadCreatedCallback(Task *task);

	void Initialize(std::thread &&thread);

	// In Windows, if the thread don't yield CPU, the main thread never got a chance to run
	// So we need to pause after a duration, and let the main thread to resume us
	void Start(std::chrono::time_point<std::chrono::steady_clock> run_until, PauseNotifier *notifier = nullptr);

	void Stop();
	void Join();

	int GetIterationCount();

public: // not copyable; only movable
	Task(const Task &task) = delete;
	Task(Task &&task);
	Task &operator=(const Task &task) = delete;
	Task &operator=(Task &&task);

private:
	enum State {
		STATE_SPAWNED,
		STATE_RUNNING,
		STATE_PAUSE,
		STATE_STOPPED
	};

private:
	void MainLoop();

	State GetState();
	void SetState(State state);

	void IncreaseIterationCount();

private: // shared (might be altered by other threads)
	std::mutex mtx_state;
	State state;

	std::mutex mtx_iterations;
	int iterations;

private: // private (will not be altered by other threads)
	std::thread thread;
	PauseNotifier *pause_notifier;
	std::chrono::time_point<std::chrono::steady_clock> run_until;

	MCTS *mcts;
};