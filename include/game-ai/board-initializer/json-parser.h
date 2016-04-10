#pragma once

#include "json/json.h"
#include "board-initializer.h"
#include "game-engine/card-database.h"
#include "game-engine/game-engine.h"
#include "deck-initializer.h"

class BoardJsonParser : public BoardInitializer
{
public:
	BoardJsonParser(Json::Value const& json) : origin_json(json)
	{
		this->first_time_parse = true;
	}

	void InitializeBoard(int rand_seed, GameEngine::Board & board)
	{
		board.SetRandomSeed(rand_seed);
		this->Parse(board);
	}

private:
	void Parse(GameEngine::Board & board)
	{
		// TODO: determine hand/deck type
		DeckInitializer player_deck, opponent_deck;
		//player_deck.SetDeck_BasicPracticeMage();
		opponent_deck.SetDeck_BasicPracticeMage();
		player_deck.SetDeck_BasicPracticeWarlock();
		//opponent_deck.SetDeck_BasicPracticeWarlock();

		this->ParsePlayer(this->origin_json["player"], board.player, player_deck);
		this->ParsePlayer(this->origin_json["opponent"], board.opponent, opponent_deck);

		// TODO: set stage
		board.SetStateToPlayerChooseBoardMove();

		if (this->first_time_parse) {
			board.DebugPrint();
			this->first_time_parse = false;
		}
	}

	void ParsePlayer(Json::Value const& json, GameEngine::Player & player, DeckInitializer const& deck_initializer) const
	{
		this->ParseCrystal(json["crystal"], player.stat.crystal);
		player.stat.fatigue_damage = json["fatigue"].asInt();

		this->ParseHero(json["hero"], player.hero);
		this->ParseMinions(json["minions"], player.minions);

		deck_initializer.InitializeHand(json["deck"]["played_cards"], json["hand"]["cards"], player.hand);

		// TODO: spell damage
		//      CURRENT_SPELLPOWER

		// TODO: secrets
		// TODO: enchantments
	}

	void ParseCrystal(Json::Value const& json, GameEngine::PlayerStat::Crystal & crystal) const
	{
		int total = json["total"].asInt();
		int used = json["used"].asInt();
		int this_turn = json["this_turn"].asInt();
		int overload = json["overload"].asInt();
		int overload_next_turn = json["overload_next_turn"].asInt();

		crystal.Set(total + this_turn - used, total + this_turn, overload, overload_next_turn);
	}

	void ParseHero(Json::Value const& json, GameEngine::Hero & hero) const
	{
		GameEngine::HeroData hero_data;

		// card_id not used
		hero_data.max_hp = json["max_hp"].asInt();
		hero_data.hp = hero_data.max_hp - json["damage"].asInt();
		hero_data.attacked_times = json["attacks_this_turn"].asInt();
		hero_data.armor = json["armor"].asInt();
		hero_data.freezed = json["status"]["frozen"].asInt() != 0;

		hero_data.hero_power.card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["hero_power"]["card_id"].asString());
		GameEngine::Card card_hero_power = GameEngine::CardDatabase::GetInstance().GetCard(hero_data.hero_power.card_id);
		hero_data.hero_power.cost = card_hero_power.cost;
		hero_data.hero_power.used_this_turn = json["hero_power"]["used"].asBool();

		// TODO: hero_data.weapon

		hero.SetHero(hero_data);
	}

	void ParseMinions(Json::Value const& json, GameEngine::Minions & minions) const
	{
		std::vector<GameEngine::MinionIterator> minion_it_map;
		minion_it_map.reserve(json.size());

		for (unsigned int i = 0; i < json.size(); ++i)
		{
			auto const& json_minion = json[i];

			bool silenced = json_minion["silenced"].asBool();

			GameEngine::MinionData minion_data = this->ParseMinion(json_minion);

			if (silenced) {
				// clear deathrattles
				minion_data.triggers_on_death.clear();

				// TODO: clear auras
			}
			else {
				// original deathrattles have been added in 'ParseMinion'
			}

			auto minion_it = minions.InsertBefore(minions.GetEndIterator(), std::move(minion_data));


			if (minion_it_map.size() != i) throw std::runtime_error("logic error");
			minion_it_map.push_back(minion_it);
		}

		for (unsigned int i = 0; i < json.size(); ++i)
		{
			auto const& json_minion = json[i];
			auto minion_it = minion_it_map[i];

			// TODO: enchantments

			// TODO: set status
			this->SetFinalMinionStatus(json_minion, minion_it);
		}
	}

	GameEngine::MinionData ParseMinion(Json::Value const& json) const
	{
		int card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(json["card_id"].asString());
		GameEngine::Card card = GameEngine::CardDatabase::GetInstance().GetCard(card_id);
		auto minion_data = GameEngine::MinionData::FromCard(card);

		minion_data.summoned_this_turn = json["summoned_this_turn"].asBool();
		minion_data.attacked_times = json["attacks_this_turn"].asInt();

		return std::move(minion_data);
	}

	void SetFinalMinionStatus(Json::Value const& json, GameEngine::MinionIterator it) const
	{
		auto & minion_data = it->GetMinionForBoardInitialization();

		// add an enchant to adjust the attack and max_hp
		int attack_boost = json["attack"].asInt() - it->GetAttack();
		int max_hp_boost = json["max_hp"].asInt() - it->GetMaxHP();
		int spell_damage_boost = json["spellpower"].asInt() - it->GetMinion().stat.GetSpellDamage();
		if (attack_boost != 0 || max_hp_boost != 0 || spell_damage_boost != 0) {
			auto enchant = new GameEngine::Enchantment_BuffMinion(attack_boost, max_hp_boost, spell_damage_boost, 0, false);
			it->enchantments.Add(std::unique_ptr<GameEngine::Enchantment_BuffMinion>(enchant));
		}

		if (json["attack"].asInt() != it->GetAttack()) throw std::runtime_error("attack not match");
		if (json["max_hp"].asInt() != it->GetMaxHP()) throw std::runtime_error("max_hp not match");
		if (json["spellpower"].asInt() != it->GetMinion().stat.GetSpellDamage()) throw std::runtime_error("spell damage not match");

		// set current hp
		minion_data.stat.SetHP(json["max_hp"].asInt() - json["damage"].asInt());

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
	}

private:
	Json::Value origin_json;
	bool first_time_parse;
};