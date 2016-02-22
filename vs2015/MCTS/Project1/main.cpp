#include <time.h>
#include <random>
#include <map>
#include <iostream>

#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"
#include "task.h"
#include "decider.h"

#include "game-engine/cards/card_FP1_007.h"

void InitializeDeck1(GameEngine::Deck &deck)
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

void InitializeHand1(GameEngine::Hand &hand)
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

void InitializeBoard(GameEngine::Board &board)
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

static void Run()
{
	constexpr int threads = 1;
	constexpr int sec_each_run = 1;
	constexpr int msec_total = 20 * 1000;

	std::mt19937 random_generator((unsigned int)time(nullptr));
	MCTS mcts[threads];
	std::vector<Task*> tasks;
	std::map<Task*, Task::PauseNotifier*> pause_notifiers;

	GameEngine::Board board;
	InitializeBoard(board);
	board.DebugPrint();

	for (int i = 0; i < threads; ++i) {
		mcts[i].Initialize(random_generator(), board);
		Task *new_task = new Task(&mcts[i]);
		new_task->Initialize(std::thread(Task::ThreadCreatedCallback, new_task));
		tasks.push_back(new_task);

		pause_notifiers[new_task] = new Task::PauseNotifier;

		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		new_task->Start(run_until, pause_notifiers[new_task]);
	}

	auto start = std::chrono::steady_clock::now();
	for (int times = 0;; times++)
	{
		std::cout << "Waiting..." << std::endl;
		std::cout.flush();
		for (const auto &task : tasks)
		{
			pause_notifiers[task]->WaitUntilPaused();
		}

		std::cout << "Deciding..." << std::endl;
		std::cout.flush();
		Decider decider;
		for (int i = 0; i < threads; ++i) {
			decider.Add(mcts[i]);
		}
		auto best_moves = decider.GetBestMoves();
		best_moves.DebugPrint();

		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		int total_iterations = 0;
		for (auto const& task : tasks) total_iterations += task->GetIterationCount();
		std::cout << "Done " << total_iterations << " iterations in " << elapsed_ms << " ms"
			<< " (Average: " << ((double)total_iterations * 1000 / elapsed_ms) << " iterations per second.)" << std::endl;
		std::cout.flush();

		if (elapsed_ms > msec_total) break;

		// start all threads for 10 second
		std::cout << "Resuming..." << std::endl;
		std::cout.flush();
		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		for (const auto &task : tasks)
		{
			task->Start(run_until, pause_notifiers[task]);
		}
	}

	for (const auto &task : tasks)
	{
		task->Stop();
	}
	for (const auto &task : tasks)
	{
		task->Join();
	}

	std::cout << "Threads stopped" << std::endl;
	std::cin.clear();

	for (const auto &task : tasks)
	{
		delete task;
	}
}

int main(void)
{
	std::cout << "Loading card database..." << std::endl;
	if (!GameEngine::CardDatabase::GetInstance().ReadFromJsonFile("../../../database/cards.json"))
		throw std::runtime_error("failed to load card data");

	Run();

	std::cout << "Press Enter to exit...";
	std::cin.get();

	return 0;
}
