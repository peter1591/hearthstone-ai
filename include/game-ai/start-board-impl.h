#pragma once

#include "game-engine/game-engine.h"

#include "start-board.h"

inline void StartBoard::InitializeDebugDeck1(GameEngine::Deck &deck)
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

inline void StartBoard::InitializeDebugHand1(GameEngine::Hand &hand)
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

inline void StartBoard::InitializeDebugBoard1(GameEngine::Board &board)
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

	InitializeDebugDeck1(board.player_deck);
	InitializeDebugHand1(board.player_hand);

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

inline StartBoard::StartBoard()
{
	InitializeDebugBoard1(this->board_debug1);
}

inline GameEngine::Board StartBoard::GetBoard(int rand_seed)
{
	return GameEngine::Board::Clone(this->board_debug1);
}