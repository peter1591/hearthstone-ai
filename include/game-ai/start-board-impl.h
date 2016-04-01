#pragma once

#include "game-engine/game-engine.h"

#include "start-board.h"

inline void StartBoard::InitializeDebugDeck1(GameEngine::Hand &hand)
{
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_043)); // Weapon: Glaivezooka
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_059)); // Weapon: Coghammer
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_112)); // Mogor the Ogre
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_066)); // Dunemaul Shaman
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_FP1_021)); // Weapon: Death's Bite
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRM_019)); // Grim Patron
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_054)); // Weapon: Ogre Warmaul
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_NEW1_018)); // Bloodsail Raider
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_409t)); // Weapon: Heavy Axe
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_002)); // Snowchugger
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_522)); // Patient Assassin
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_222)); // Stormwind Champion
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_NEW1_010)); // Raymond Swanland
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_412)); // Raging Worgen
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_390)); // Tauren Warrior
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_051)); // Warbot
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion
}

inline void StartBoard::InitializeDebugHand1(GameEngine::Hand &hand)
{
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	//hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	//hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	//hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	//hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion

	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
}

inline void StartBoard::InitializeDebugBoard1(GameEngine::Board &board)
{
	board.player.stat.crystal.Set(1, 1, 0, 0);
	board.player.stat.fatigue_damage = 0;

	board.opponent.stat.crystal.Set(0, 0, 0, 0);
	board.opponent.stat.fatigue_damage = 0;

	GameEngine::BoardObjects::HeroData player_hero, opponent_hero;

	player_hero.hp = 20;
	player_hero.armor = 0;
	player_hero.weapon.Clear();

	opponent_hero.hp = 20;
	opponent_hero.armor = 0;
	opponent_hero.weapon.Clear();

	board.object_manager.SetHero(player_hero, opponent_hero);

	InitializeDebugDeck1(board.player.hand);
	InitializeDebugHand1(board.player.hand);

	InitializeDebugDeck1(board.opponent.hand);
	InitializeDebugHand1(board.opponent.hand);

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(CARD_ID_FP1_007, 2, 2, 2);
		auto & added_minion = board.player.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion));
		added_minion.AddOnDeathTrigger(GameEngine::BoardObjects::Minion::OnDeathTrigger(GameEngine::Cards::Card_FP1_007::Deathrattle));
		added_minion.TurnStart(true);
	}

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(222, 30, 2, 2);
		board.opponent.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion)).TurnStart(true);
	}

	{
		auto minion = GameEngine::BoardObjects::MinionData();
		minion.Set(222, 10, 7, 7);
		board.opponent.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion)).TurnStart(true);
	}

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
}

inline StartBoard::StartBoard()
{
	InitializeDebugBoard1(this->board_debug1);
}

inline GameEngine::Board StartBoard::GetBoard(int rand_seed) const
{
	GameEngine::Board board = GameEngine::Board::Clone(this->board_debug1);
	board.SetRandomSeed(rand_seed);
	return std::move(board);
}