#pragma once

#include "json/json.h"
#include "board-initializer.h"
#include "game-engine/card-database.h"

class BoardJsonParser : public BoardInitializer
{
public:
	BoardJsonParser(Json::Value const& json) : json(json)
	{
	}

	GameEngine::Board GetBoard(int rand_seed) const
	{
		return std::move(this->Parse());
	}

private:
	GameEngine::Board Parse() const
	{
		GameEngine::Board board;

		// parse
		this->ParsePlayer(this->json["player"], board.player);
		this->ParsePlayer(this->json["opponent"], board.opponent);

		return std::move(board);
	}

	void ParsePlayer(Json::Value const& json, GameEngine::Player & player) const
	{
		this->ParseCrystal(json["crystal"], player.stat.crystal);

		player.stat.fatigue_damage = json["fatigue"].asInt();

		// TODO: spell damage

		// TODO: secrets

		// TODO: hand

		this->ParseHero(json["hero"], player.hero);

		// TODO: minions

		// TODO: enchantments
	}

	void ParseCrystal(Json::Value const& json, GameEngine::PlayerStat::Crystal & crystal) const
	{
		int total = json["total"].asInt();
		int used = json["used"].asInt();
		int this_turn = json["this_turn"].asInt();
		int overload = json["overload"].asInt();
		int overload_next_turn = json["overload_next_turn"].asInt();

		crystal.Set(total - used, total, overload, overload_next_turn);
	}

	void ParseHero(Json::Value const& json, GameEngine::Hero & hero) const
	{
		GameEngine::HeroData hero_data;

		// card_id not used
		hero_data.hp = json["max_hp"].asInt() - json["damage"].asInt();
		hero_data.attacked_times = json["attacks_this_turn"].asInt();
		hero_data.armor = json["armor"].asInt();
		hero_data.freezed = json["status"]["frozen"].asInt() != 0;
		hero_data.hero_power.card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["hero_power"]["card_id"].asString());
		hero_data.hero_power.used_this_turn = json["hero_power"]["used"].asBool();

		// TODO: hero_data.weapon
	}

private:
	Json::Value json;
};