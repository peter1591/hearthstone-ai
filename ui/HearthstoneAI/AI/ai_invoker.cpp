#include <random>
#include <thread>

#include "game-ai.h"
#include "task.h"

#include "ai_invoker.h"

AIInvoker::AIInvoker()
{
	this->state = STATE_NOT_SPAWNED;
}

static void InitializeDeck1(GameEngine::Deck &deck)
{
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_043)); // Weapon: Glaivezooka
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_059)); // Weapon: Coghammer
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_112)); // Mogor the Ogre
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_066)); // Dunemaul Shaman
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_FP1_021)); // Weapon: Death's Bite
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRM_019)); // Grim Patron
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_054)); // Weapon: Ogre Warmaul
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_NEW1_018)); // Bloodsail Raider
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_409t)); // Weapon: Heavy Axe
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_002)); // Snowchugger
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_522)); // Patient Assassin
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_222)); // Stormwind Champion
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_NEW1_010)); // Raymond Swanland
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_412)); // Raging Worgen
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_390)); // Tauren Warrior
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_051)); // Warbot
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	deck.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion
}

static void InitializeHand1(GameEngine::Hand &hand)
{
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
																					 //hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
																					 //hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
																					 //hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
																					 //hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion

	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
	hand.AddCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
}

static void InitializeBoard(GameEngine::Board &board)
{
	board.player_stat.crystal.Set(1, 1, 0, 0);
	board.player_stat.fatigue_damage = 0;

	board.opponent_stat.crystal.Set(0, 0, 0, 0);
	board.opponent_stat.fatigue_damage = 0;

	GameEngine::BoardObjects::HeroData player_hero, opponent_hero;

	player_hero.hp = 20;
	player_hero.armor = 0;
	player_hero.weapon.Clear();

	opponent_hero.hp = 20;
	opponent_hero.armor = 0;
	opponent_hero.weapon.Clear();

	board.object_manager.SetHero(player_hero, opponent_hero);

	board.opponent_cards.Set(4, 26);

	InitializeDeck1(board.player_deck);
	InitializeHand1(board.player_hand);

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(CARD_ID_FP1_007, 2, 2, 2);
		auto & added_minion = board.object_manager.player_minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion));
		added_minion.AddOnDeathTrigger(GameEngine::BoardObjects::Minion::OnDeathTrigger(GameEngine::Cards::Card_FP1_007::Deathrattle));
		added_minion.TurnStart(true);
	}

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(222, 30, 2, 2);
		board.object_manager.opponent_minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion)).TurnStart(true);
	}

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(222, 10, 7, 7);
		board.object_manager.opponent_minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion)).TurnStart(true);
	}

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
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
		Job * new_job = new Job();
		new_job->game = game;
		instance->DiscardAndSetPendingJob(new_job);
	});
}

void AIInvoker::CancelAllTasks()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([] (AIInvoker* instance) {
		instance->DiscardAndSetPendingJob(nullptr);
		instance->StopCurrentJob();
	});
}

void AIInvoker::GenerateCurrentBestMoves()
{
	std::unique_lock<std::mutex> lock(this->mtx_pending_operations);
	this->pending_operations.push_back([] (AIInvoker* instance) {
		instance->GenerateCurrentBestMoves_Internal();
	});
}

void AIInvoker::HandleCurrentJob()
{
	constexpr int threads = 4;
	constexpr int sec_each_run = 1;
	constexpr int msec_total = 70 * 1000;

	bool just_initailized = false;

	if (this->mcts.empty())
	{
		// initialize the job
		std::mt19937 random_generator((unsigned int)time(nullptr));

		GameEngine::Board board;
		InitializeBoard(board); // TODO: obtain board from json
		board.DebugPrint();

		for (int i = 0; i < threads; ++i) {
			MCTS * new_mcts = new MCTS();
			new_mcts->Initialize(random_generator(), board);
			this->mcts.push_back(new_mcts);

			Task *new_task = new Task(new_mcts);
			new_task->Initialize(std::thread(Task::ThreadCreatedCallback, new_task));

			this->tasks.push_back(new_task);
		}

		this->start_time = std::chrono::steady_clock::now();

		just_initailized = true;
	}

	if (!just_initailized) {
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->start_time).count();
		int total_iterations = 0;
		for (auto const& task : this->tasks) total_iterations += task->GetIterationCount();
		std::cerr << "Done " << total_iterations << " iterations in " << elapsed_ms << " ms"
			<< " (Average: " << ((double)total_iterations * 1000 / elapsed_ms) << " iterations per second.)" << std::endl;
		std::cerr.flush();

		//if (elapsed_ms > msec_total)
		//{
		//	std::cerr << "ERROR: TIMEOUT!!!!" << std::endl;
		//	this->StopCurrentJob();
		//	return;
		//}
	}

	if (!just_initailized ) {
		for (const auto &task : this->tasks) {
			pause_notifiers[task]->WaitUntilPaused();
		}
	}

	// start all threads
	auto run_until = std::chrono::steady_clock::now() +
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
	for (const auto &task : this->tasks)
	{
		if (pause_notifiers.find(task) == pause_notifiers.end()) {
			pause_notifiers[task] = new Task::PauseNotifier();
		}
		task->Start(run_until, pause_notifiers[task]);
	}
}

void AIInvoker::StopCurrentJob()
{
	for (const auto &task : this->tasks)
	{
		task->Stop();
	}
	for (const auto &task : this->tasks)
	{
		task->Join();
	}

	if (this->current_job) delete this->current_job;
	this->current_job = nullptr;

	for (const auto &task : this->tasks)
	{
		delete task;
		if (this->pause_notifiers.find(task) != this->pause_notifiers.end()) {
			delete this->pause_notifiers[task];
		}
		this->pause_notifiers.erase(task);
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
	if (this->mcts.empty()) return;

	Decider decider;
	for (auto each_mcts : this->mcts) {
		decider.Add(*each_mcts);
	}
	auto best_moves = decider.GetBestMoves();
	best_moves.DebugPrint();
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
			if (this->current_job) {
				this->StopCurrentJob();
			}
			this->current_job = new_job;
		}

		if (this->current_job) this->HandleCurrentJob();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

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
