#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <list>
#include <functional>

#include "json/value.h"
#include "game-ai.h"
#include "task.h"

class AIInvoker
{
public:
	AIInvoker();

	bool Initialize();

	void CreateNewTask(Json::Value game);
	void CancelAllTasks();
	void GenerateCurrentBestMoves();
	void GenerateBestMovesAndFinish();

	void Cleanup();

private:
	enum State
	{
		STATE_NOT_SPAWNED,
		STATE_RUNNING,
		STATE_STOPPED
	};

	class Job
	{
	public:
		Json::Value game;
	};

private: // non-thread-safe functions
	void MainLoop();
	static void ThreadCallBack(AIInvoker * instance);

	void ProcessPendingOperations();

	void HandleCurrentJob();
	void StopCurrentJob();

	void GenerateCurrentBestMoves_Internal();

private: // thread-safe functions
	void SetState(State state);
	State GetState();

	void DiscardAndSetPendingJob(Job * new_job);
	Job * GetAndClearPendingJob();

private:
	std::thread main_thread;

	std::mutex mtx_state;
	State state;

	std::mutex mtx_pending_job;
	Job * pending_job;

	std::mutex mtx_pending_operations;
	std::list<std::function<void(AIInvoker*)>> pending_operations;

	Job * current_job;

	bool abort_flag;
	bool flag_generate_best_move;

private: // context for current job
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::vector<MCTS*> mcts;
	std::vector<Task*> tasks;
	std::map<Task*, Task::PauseNotifier*> pause_notifiers;
};