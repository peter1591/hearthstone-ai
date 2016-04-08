#pragma once

#include "json/json.h"
#include "board-initializer.h"
#include "game-engine/card-database.h"
#include "game-engine/game-engine.h"

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

		this->ParseHero(json["hero"], player.hero);
		this->ParseMinions(json["minions"], player.minions);

		// TODO: spell damage
		// TODO: secrets
		// TODO: hand
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

	void ParseMinions(Json::Value const& json, GameEngine::Minions & minions) const
	{
		for (auto const& json_minion : json)
		{
			bool silenced = json_minion["silenced"].asBool();

			GameEngine::MinionData minion_data = this->ParseMinion(json_minion);

			if (!silenced) {
				// deathrattles
				auto deathrattle = GameEngine::Cards::CardCallbackManager::GetDeathrattle(minion_data.card_id);
				if (deathrattle) minion_data.triggers_on_death.push_back(GameEngine::MinionData::OnDeathTrigger(deathrattle));

				// TODO: auras
			}

			auto minion_it = minions.InsertBefore(minions.GetEndIterator(), std::move(minion_data));

			// TODO: enchantments

			// TODO: set status
			this->SetFinalMinionStatus(json_minion, minion_it);
		}
	}

	GameEngine::MinionData ParseMinion(Json::Value const& json) const
	{
		int card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["card_id"].asString());
		int attack = json["attack"].asInt();
		int max_hp = json["max_hp"].asInt();
		int hp = max_hp - json["damage"].asInt();
		int spell_damage = 0; // TODO

		return GameEngine::MinionData(card_id, attack, hp, max_hp, spell_damage);
	}

	void SetFinalMinionStatus(Json::Value const& json, GameEngine::MinionIterator it) const
	{
		auto minion_data = it->GetMinionForBoardInitialization();

		Json::Value const& json_status = json["status"];

		if (json_status["charge"].asInt() > 0) minion_data.stat.SetCharge();
		if (json_status["taunt"].asInt() > 0) minion_data.stat.SetTaunt();
		if (json_status["divine_shield"].asInt() > 0) minion_data.stat.SetShield();
		if (json_status["stealth"].asInt() > 0) minion_data.stat.SetStealth();
		if (json_status["forgetful"].asInt() > 0) minion_data.stat.SetForgetful();
		if (json_status["freeze"].asInt() > 0) minion_data.stat.SetFreezeAttacker();
		if (json_status["frozen"].asInt() > 0) minion_data.stat.SetFreezed();
		if (json_status["poisonous"].asInt() > 0) minion_data.stat.SetPoisonous();
		if (json_status["windfury"].asInt() > 0) minion_data.stat.SetWindFury();

		minion_data.stat.SetAttack(json["attack"].asInt());
		minion_data.stat.SetMaxHP(json["max_hp"].asInt());
		minion_data.stat.SetHP(json["max_hp"].asInt() - json["damage"].asInt());
		// TODO: spell damage
	}

private:
	Json::Value json;
};