#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "Cards/Database.h"
#include "UI/AIController.h"
#include "UI/Decks.h"
#include "UI/Board/Board.h"
#include "UI/GameEngineLogger.h"
#include "MCTS/TestStateBuilder.h"

namespace ui
{
	class BoardGetter
	{
	private:
		class MyRandomGenerator : public state::IRandomGenerator
		{
		public:
			MyRandomGenerator(int seed) : random_(seed) {}

			int Get(int exclusive_max)
			{
				return random_() % exclusive_max;
			}

			size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

			int Get(int min, int max)
			{
				return min + Get(max - min + 1);
			}

		public:
			std::mt19937 random_;
		};

	public:
		BoardGetter(GameEngineLogger & logger) :
			logger_(logger), board_raw_()
		{}

		int ResetBoard()
		{
			std::lock_guard<std::shared_mutex> lock(lock_);
			board_raw_.clear();
			need_restart_ai_ = true;
			return 0;
		}

		int UpdateBoard(std::string const& board_str)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);

			if (board_raw_ == board_str) return 0;

			logger_.Log("Updating board.");
			board_raw_ = board_str;

			// TODO: reuse MCTS tree
			return 0;
		}

		int PrepareToRun(ui::AIController * controller, bool * restart_ai)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);

			Json::Reader reader;
			std::stringstream ss(board_raw_);
			Json::Value board;
			if (!reader.parse(ss, board)) {
				logger_.Log("Failed to parse board.");
				return -1;
			}

			board_.Parse(board);

			if (need_restart_ai_) {
				*restart_ai = true;
				return 0;
			}

			// TODO: reuse last MCTS tree
			(void)controller;
			*restart_ai = true;

			return 0;
		}

		state::State GetStartBoard(int seed)
		{
			std::shared_lock<std::shared_mutex> lock(lock_);

			// TODO: should only read board_
			state::State state;
			Json::Reader reader;
			std::stringstream ss(board_raw_);
			Json::Value board;
			if (!reader.parse(ss, board)) {
				logger_.Log("Failed to parse board.");
				return state;
			}

			MyRandomGenerator random(seed);

			MakePlayer(state::kPlayerFirst, state, random, board["player"]);
			MakePlayer(state::kPlayerSecond, state, random, board["opponent"]);
			state.GetMutableCurrentPlayerId().SetFirst(); // AI is helping first player, and should not waiting for an action
			state.SetTurn(board["turn"].asInt());

			return state;
		}

	private:
		Cards::CardId GetCardId(std::string const& card_id)
		{
			auto const& container = Cards::Database::GetInstance().GetIdMap();
			auto it = container.find(card_id);
			if (it == container.end()) {
				return Cards::kInvalidCardId;
			}
			return (Cards::CardId)it->second;
		}

		void MakePlayer(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Json::Value const& json)
		{
			MakeHero(player, state, json["hero"]);
			MakeHeroPower(player, state, json["hero"]["hero_power"]);
			MakeDeck(player, state, random, json["deck"]);
			MakeHand(player, state, random, json["hand"]);
			MakeMinions(player, state, random, json["minions"]);
			// TODO: enchantments
			// TODO: weapon
			// TODO: secrets

			state.GetBoard().Get(player).SetFatigueDamage(json["fatigue"].asInt());
			MakeResource(state.GetBoard().Get(player).GetResource(), json["crystal"]);
		}

		void ApplyStatus(state::Cards::CardData & raw_card, Json::Value const& json)
		{
			raw_card.enchanted_states.charge = json["charge"].asBool();
			raw_card.taunt = json["taunt"].asBool();
			raw_card.shielded = json["divine_shield"].asBool();
			raw_card.enchanted_states.stealth = json["stealth"].asBool();
			// TODO: forgetful
			raw_card.enchanted_states.freeze_attack = json["freeze"].asBool();
			raw_card.freezed = json["frozen"].asBool();
			raw_card.enchanted_states.poisonous = json["poisonous"].asBool();

			int max_attacks_per_turn = 1;
			if (json["windfury"].asBool()) max_attacks_per_turn = 2;
			raw_card.enchanted_states.max_attacks_per_turn = max_attacks_per_turn;
		}

		void AddMinion(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Json::Value const& json, int pos)
		{
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(
				GetCardId(json["card_id"].asString()));
			raw_card.enchanted_states.player = player;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

			raw_card.enchanted_states.max_hp = json["max_hp"].asInt();
			raw_card.damaged = json["damaged"].asInt();
			raw_card.enchanted_states.attack = json["attack"].asInt();
			raw_card.num_attacks_this_turn = json["attacks_this_turn"].asInt();
			// TODO: exhausted (needed?)
			raw_card.silenced = json["silenced"].asBool();
			raw_card.enchanted_states.spell_damage = json["spellpower"].asInt();
			raw_card.just_played = json["summoned_this_turn"].asBool();
			// TODO: enchantments
			ApplyStatus(raw_card, json["status"]);

			raw_card.zone = state::kCardZoneNewlyCreated;
			state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
			state.GetZoneChanger<state::kCardTypeMinion, state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZonePlay>(player, pos);
		}

		void MakeMinions(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Json::Value const& json)
		{
			for (Json::ArrayIndex idx = 0; idx < json.size(); ++idx)
			{
				AddMinion(player, state, random, json[idx], (int)idx);
			}
		}

		void MakeResource(state::board::PlayerResource & resource, Json::Value const& json)
		{
			resource.SetCurrent(json["this_turn"].asInt());
			resource.SetCurrentOverloaded(json["overload"].asInt());
			resource.SetTotal(json["total"].asInt());
			resource.SetNextOverload(json["overload_next_turn"].asInt());
		}

		void MakeHero(state::PlayerIdentifier player, state::State & state, Json::Value const& json)
		{
			// TODO: mark ctor of state::Cards::CardData as private, to make it only constructible from CardDispatcher::CreateInstance
			
			std::string card_id = json["card_id"].asString();
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(
				GetCardId(card_id));
			
			raw_card.enchanted_states.player = player;
			assert(raw_card.card_type == state::kCardTypeHero);
			raw_card.zone = state::kCardZoneNewlyCreated;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

			raw_card.enchanted_states.max_hp = json["max_hp"].asInt();
			raw_card.damaged = json["damage"].asInt();
			raw_card.armor = json["armor"].asInt();
			raw_card.enchanted_states.attack = json["attack"].asInt();
			raw_card.num_attacks_this_turn = json["attacks_this_turn"].asInt();
			ApplyStatus(raw_card, json["status"]);

			// TODO: enchantments

			state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
			state.GetZoneChanger<state::kCardTypeHero, state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZonePlay>(player);
		}

		void MakeHeroPower(state::PlayerIdentifier player, state::State & state, Json::Value const& json)
		{
			std::string card_id = json["card_id"].asString();
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(
				GetCardId(card_id));
			assert(raw_card.card_type == state::kCardTypeHeroPower);
			raw_card.used_this_turn = json["used"].asBool();

			raw_card.zone = state::kCardZoneNewlyCreated;
			state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
			state.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZonePlay>(player);
		}

		void PushBackDeckCard(Cards::CardId id, state::IRandomGenerator & random, state::State & state, state::PlayerIdentifier player)
		{
			int deck_count = (int)state.GetBoard().Get(player).deck_.Size();
			state.GetBoard().Get(player).deck_.ShuffleAdd(id, random);
			++deck_count;
			assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
		}

		void MakeDeck(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Json::Value const& json)
		{
			auto deck1 = ui::Decks::GetDeck("InnKeeperBasicMage"); // TODO: read deck info from json
			// TODO: remove drawn cards
			for (auto const& card_name : deck1) {
				Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
				PushBackDeckCard(card_id, random, state, player);
			}
		}

		state::CardRef AddHandCard(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Cards::CardId card_id)
		{
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(card_id);
			raw_card.enchanted_states.player = player;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);
			raw_card.zone = state::kCardZoneNewlyCreated;

			auto ref = state.AddCard(state::Cards::Card(raw_card));

			int hand_count = (int)state.GetBoard().Get(player).hand_.Size();
			state.GetZoneChanger<state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZoneHand>(player);

			assert(state.GetCardsManager().Get(ref).GetCardId() == card_id);
			assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
			if (hand_count == 10) {
				assert(state.GetBoard().Get(player).hand_.Size() == 10);
				assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneGraveyard);
			}
			else {
				++hand_count;
				assert((int)state.GetBoard().Get(player).hand_.Size() == hand_count);
				assert(state.GetBoard().Get(player).hand_.Get(hand_count - 1) == ref);
				assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneHand);
				assert(state.GetCardsManager().Get(ref).GetZonePosition() == (hand_count - 1));
			}

			return ref;
		}

		void MakeHand(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, Json::Value const& json)
		{
			auto const& cards = json["cards"];
			for (Json::ArrayIndex idx = 0; idx < cards.size(); ++idx) {
				auto card_id = GetCardId(cards[idx].asString());
				if (card_id == Cards::kInvalidCardId) {
					// TODO: get a random card from deck
				}
				else {
					AddHandCard(player, state, random, card_id);
				}
			}
		}

	private:
		std::shared_mutex lock_;
		GameEngineLogger & logger_;
		bool need_restart_ai_;
		std::string board_raw_; // TODO: remove this one, parse all necessary info to board
		board::Board board_;
	};
}