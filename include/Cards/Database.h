#pragma once

#include <assert.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include "json/json.h"

#include "state/Types.h"
#include "state/Cards/CardData.h"

namespace Cards
{
	class Database
	{
	public:
		struct CardData
		{
			CardData() :
				card_id(-1), card_type(state::kCardTypeInvalid), card_race(state::kCardRaceInvalid),
				card_rarity(state::kCardRarityInvalid), card_set(state::kCardSetInvalid),
				cost(-1), attack(-1), max_hp(-1), collectible(false)
			{}

			int card_id;
			state::CardType card_type;
			state::CardRace card_race;
			state::CardRarity card_rarity;
			state::CardSet card_set;

			int cost;
			int attack;
			int max_hp;

			bool collectible;

			static constexpr int kFieldChangeId = 2; // modify this if any field changed. This helps to track which codes should be modified accordingly.
		};

		static Database & GetInstance()
		{
			static Database instance;
			return instance;
		}

	public:
		static std::unordered_set<state::CardSet> const& GetAllCardSets() {
			static std::unordered_set<state::CardSet> card_sets = {
				state::kCardSetCore,
				state::kCardSetExpert1,
				state::kCardSetOldGods,
				state::kCardSetKara,
				state::kCardSetGangs,
				state::kCardSetUngoro
			};
			return card_sets;
		}

		bool Initialize(std::string const& path) {
			return LoadJsonFile(path);
		}

		std::unordered_map<std::string, int> GetIdMap() const { return origin_id_map_; }

		CardData const& Get(int id)
		{
			assert(id >= 0);
			assert(id < final_cards_size_);
			return final_cards_[id];
		}

	public:
		enum CachedCardsTypes {
			kCollectibleCards,
			kCachedCardsTypesCount
		};
		std::vector<int> const& GetCachedCards(CachedCardsTypes cached_cards_type) {
			return cached_cards[cached_cards_type];
		}

	private:
		Database() : final_cards_(nullptr), final_cards_size_(0) { }

		bool LoadJsonFile(std::string const& path)
		{
			Json::Reader reader;
			Json::Value cards_json;

			std::ifstream cards_file(path);

			if (reader.parse(cards_file, cards_json, false) == false) return false;

			return this->ReadFromJson(cards_json);
		}

		bool ReadFromJson(Json::Value const & cards_json)
		{
			if (cards_json.isArray() == false) return false;

			std::vector<CardData> cards;

			// Reserve id = 0
			cards.push_back(CardData());

			origin_id_map_.clear();
			for (auto const& card_json : cards_json) {
				this->AddCard(card_json, cards);
			}

			if (final_cards_) { delete[] final_cards_; }

			final_cards_size_ = (int)cards.size();
			final_cards_ = new CardData[final_cards_size_];

			// Copy to raw array to support lock-free access
			for (int i = 0; i < cards.size(); ++i) {
				final_cards_[i] = cards[i];
			}

			return true;
		}

		state::CardRace GetCardRace(Json::Value const& json)
		{
			const std::string race = json["race"].asString();

			if (race == "BEAST") return state::kCardRaceBeast;
			if (race == "MECHANICAL") return state::kCardRaceMech;
			if (race == "DEMON") return state::kCardRaceDemon;
			if (race == "DRAGON") return state::kCardRaceDragon;
			if (race == "MURLOC") return state::kCardRaceMurloc;
			if (race == "TOTEM") return state::kCardRaceTotem;
			if (race == "PIRATE") return state::kCardRacePirate;
			if (race == "ELEMENTAL") return state::kCardRaceElemental;
			if (race == "ORC") return state::kCardRaceOrc;

			if (race == "") return state::kCardRaceInvalid;

			throw std::exception("unknown race");
		}

		state::CardSet GetCardSet(Json::Value const& json)
		{
			const std::string set = json["set"].asString();

			if (set == "CORE") return state::kCardSetCore;
			if (set == "EXPERT1") return state::kCardSetExpert1;
			if (set == "HOF") return state::kCardSetHOF;

			if (set == "BRM") return state::kCardSetBRM;
			if (set == "TGT") return state::kCardSetTGT;
			if (set == "GVG") return state::kCardSetGVG;
			if (set == "NAXX") return state::kCardSetNaxx;
			if (set == "LOE") return state::kCardSetLOE;

			if (set == "OG") return state::kCardSetOldGods;
			if (set == "KARA") return state::kCardSetKara;
			if (set == "GANGS") return state::kCardSetGangs;
			if (set == "UNGORO") return state::kCardSetUngoro;

			if (set == "TB") return state::kCardSetTB;

			if (set == "CHEAT") return state::kCardSetInvalid;
			if (set == "MISSIONS") return state::kCardSetInvalid;
			if (set == "CREDITS") return state::kCardSetInvalid;
			if (set == "HERO_SKINS") return state::kCardSetInvalid;
			throw std::exception("unknown set");
		}

		state::CardRarity GetCardRarity(Json::Value const& json)
		{
			const std::string rarity = json["rarity"].asString();

			if (rarity == "COMMON") return state::kCardRarityCommon;
			if (rarity == "RARE") return state::kCardRarityRare;
			if (rarity == "EPIC") return state::kCardRarityEpic;
			if (rarity == "LEGENDARY") return state::kCardRarityLegendary;

			if (rarity == "FREE") return state::kCardRarityInvalid;
			if (rarity == "") return state::kCardRarityInvalid;

			throw std::exception("unknown rarity");
		}

		void AddCard(Json::Value const& json, std::vector<CardData> & cards)
		{
			const std::string origin_id = json["id"].asString();
			const std::string type = json["type"].asString();

			if (origin_id == "PlaceholderCard") return;

			if (origin_id_map_.find(origin_id) != origin_id_map_.end()) {
				throw std::exception("Card ID string collision.");
			}

			CardData new_card;
			new_card.card_id = (int)cards.size();

			new_card.cost = json["cost"].asInt();
			new_card.card_rarity = GetCardRarity(json);

			if (json.isMember("set") == false) throw std::exception("set field not exists");
			new_card.card_set = GetCardSet(json);

			if (GetAllCardSets().find(new_card.card_set) == GetAllCardSets().end()) {
				return;
			}

			if (json.isMember("collectible")) {
				new_card.collectible = json["collectible"].asBool();
			}

			if (type == "MINION") {
				new_card.card_type = state::kCardTypeMinion;
				new_card.card_race = GetCardRace(json);
				new_card.attack = json["attack"].asInt();
				new_card.max_hp = json["health"].asInt();
			}
			else if (type == "SPELL") {
				new_card.card_type = state::kCardTypeSpell;
			}
			else if (type == "WEAPON") {
				new_card.card_type = state::kCardTypeWeapon;
				new_card.attack = json["attack"].asInt();
				new_card.max_hp = json["durability"].asInt();
			}
			else if (type == "HERO_POWER") {
				new_card.card_type = state::kCardTypeHeroPower;
			}
			else if (type == "ENCHANTMENT") {
				new_card.card_type = state::kCardTypeEnchantment;
			}
			else {
				return; // ignored
			}

			origin_id_map_[origin_id] = new_card.card_id;
			cards.push_back(new_card);

			ProcessCachedCardsTypes(new_card);
		}

		void ProcessCachedCardsTypes(CardData const& new_card) {
			if (new_card.collectible) {
				cached_cards[kCollectibleCards].push_back(new_card.card_id);
			}
		}

	private:
		CardData * final_cards_; // Raw array to support lock-free access
		int final_cards_size_;

		std::unordered_map<std::string, int> origin_id_map_;

		std::array<std::vector<int>, kCachedCardsTypesCount> cached_cards;
	};
}