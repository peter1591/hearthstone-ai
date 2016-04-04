#pragma once

#include "game-engine/game-engine.h"

#include "start-board.h"

inline void StartBoard::InitializeDebugBoard1_PlayerHand(GameEngine::Hand &hand)
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

	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_029)); // Leper Gnome
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_023)); // arcane intellct

	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
}

inline void StartBoard::InitializeDebugBoard1_OpponentHand(GameEngine::Hand &hand)
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
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_120)); // 223
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_051)); // both player spell+2
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_AT_117)); // +2/+2 if spell>0

	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_142)); // 222 spell+1
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_584)); // adjacent spell+1
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_GVG_092t)); // 111
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion
}

inline void StartBoard::InitializeDebugBoard3_Hand(GameEngine::Hand &hand)
{
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_009t)); // 111 [TAUNT]
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_BRMA15_4)); // 111 [CHARGE]
	hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_189)); // 111 Elven Archer

	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_222)); // Stormwind Champion
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_LOE_051)); // both player spell+2
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_142)); // 222 spell+1
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_EX1_584)); // adjacent spell+1
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_188)); // 121 Abusive Argant
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_025)); // arcane explosion
	hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(CARD_ID_CS2_023)); // arcane intellct
}

inline void StartBoard::InitializeDebugBoard1(GameEngine::Board &board)
{
	board.player.stat.crystal.Set(1, 1, 0, 0);
	board.player.stat.fatigue_damage = 0;

	board.opponent.stat.crystal.Set(0, 0, 0, 0);
	board.opponent.stat.fatigue_damage = 0;

	GameEngine::HeroData player_hero;
	player_hero.hp = 20;
	player_hero.armor = 0;
	player_hero.weapon.Clear();

	GameEngine::HeroData opponent_hero;
	opponent_hero.hp = 20;
	opponent_hero.armor = 0;
	opponent_hero.weapon.Clear();

	board.player.hero.SetHero(player_hero);
	board.opponent.hero.SetHero(opponent_hero);

	InitializeDebugBoard1_PlayerHand(board.player.hand);
	InitializeDebugBoard1_OpponentHand(board.opponent.hand);

	{
		auto minion = GameEngine::MinionData(CARD_ID_FP1_007, 2, 2, 2, 0);
		auto added_minion = board.player.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion));
		added_minion->AddOnDeathTrigger(GameEngine::Minion::OnDeathTrigger(GameEngine::Cards::Card_FP1_007::Deathrattle));
		added_minion->TurnStart(true);
	}

	{
		auto minion = GameEngine::MinionData(9999999, 30, 2, 2, 0);
		board.opponent.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion))->TurnStart(true);
	}

	{
		auto minion = GameEngine::MinionData(9999999, 10, 1, 7, 0);
		board.opponent.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion))->TurnStart(true);
	}

	board.SetStateToPlayerChooseBoardMove();
}

inline void StartBoard::InitializeDebugBoard2(GameEngine::Board &board)
{
	board.player.stat.crystal.Set(0, 0, 0, 0);
	board.player.stat.fatigue_damage = 0;

	board.opponent.stat.crystal.Set(10, 10, 0, 0);
	board.opponent.stat.fatigue_damage = 0;

	GameEngine::HeroData player_hero;
	player_hero.hp = 20;
	player_hero.armor = 0;
	player_hero.weapon.Clear();

	GameEngine::HeroData opponent_hero;
	opponent_hero.hp = 20;
	opponent_hero.armor = 0;
	opponent_hero.weapon.Clear();

	board.player.hero.SetHero(player_hero);
	board.opponent.hero.SetHero(opponent_hero);

	InitializeDebugBoard1_PlayerHand(board.player.hand);
	InitializeDebugBoard1_OpponentHand(board.opponent.hand);

	{
		auto minion = GameEngine::MinionData(CARD_ID_FP1_007, 2, 2, 2, 0);
		auto added_minion = board.opponent.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_OPPONENT_SIDE),
			std::move(minion));
		added_minion->AddOnDeathTrigger(GameEngine::Minion::OnDeathTrigger(GameEngine::Cards::Card_FP1_007::Deathrattle));
		added_minion->TurnStart(true);
	}

	{
		auto minion = GameEngine::MinionData(999999, 30, 2, 2, 0);
		board.player.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion))->TurnStart(true);
	}

	{
		auto minion = GameEngine::MinionData(999999, 10, 1, 7, 0);
		board.player.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion))->TurnStart(true);
	}

	board.SetStateToPlayerChooseBoardMove();
}

inline void StartBoard::InitializeDebugBoard3(GameEngine::Board &board)
{
	board.player.stat.crystal.Set(10, 10, 0, 0);
	board.player.stat.fatigue_damage = 0;

	board.opponent.stat.crystal.Set(10, 10, 0, 0);
	board.opponent.stat.fatigue_damage = 0;

	GameEngine::HeroData player_hero;
	player_hero.hp = 20;
	player_hero.armor = 0;
	player_hero.weapon.Clear();

	GameEngine::HeroData opponent_hero;
	opponent_hero.hp = 20;
	opponent_hero.armor = 0;
	opponent_hero.weapon.Clear();

	board.player.hero.SetHero(player_hero);
	board.opponent.hero.SetHero(opponent_hero);

	InitializeDebugBoard3_Hand(board.player.hand);
	InitializeDebugBoard3_Hand(board.opponent.hand);

	{
		auto minion = GameEngine::MinionData(9999999, 10, 7, 7, 0);
		board.player.minions.InsertBefore(
			board.object_manager.GetMinionIteratorAtBeginOfSide(GameEngine::SLOT_PLAYER_SIDE),
			std::move(minion))->TurnStart(true);
	}

	board.SetStateToPlayerChooseBoardMove();
}

inline StartBoard::StartBoard()
{
	//InitializeDebugBoard1(this->board_debug1);
	InitializeDebugBoard2(this->board_debug1);
	//InitializeDebugBoard3(this->board_debug1);
}

inline GameEngine::Board StartBoard::GetBoard(int rand_seed) const
{
	GameEngine::Board board = GameEngine::Board::Clone(this->board_debug1);
	board.SetRandomSeed(rand_seed);
	return std::move(board);
}