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
#include "UI/Board/UnknownCards.h"
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
			logger_(logger), board_raw_(),
			board_(),
			first_unknown_cards_sets_mgr_(board_.GetUnknownCardsSets(1)),
			second_unknown_cards_sets_mgr_(board_.GetUnknownCardsSets(2))
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

		int PrepareToRun(std::unique_ptr<ui::AIController> & controller, int seed)
		{
			std::lock_guard<std::shared_mutex> lock(lock_);

			Json::Reader reader;
			Json::Value json_board;
			std::stringstream ss(board_raw_);
			if (!reader.parse(ss, json_board)) {
				logger_.Log("Failed to parse board.");
				return -1;
			}

			int player_entity_id = json_board["player"]["entity_id"].asInt();
			int opponent_entity_id = json_board["opponent"]["entity_id"].asInt();

			board_.Reset();

			std::string player_deck_type = "InnKeeperBasicMage";
			std::vector<Cards::CardId> player_deck_cards;
			for (auto const& card_name : Decks::GetDeck(player_deck_type)) {
				Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
				player_deck_cards.push_back(card_id);
			}
			board_.SetDeckCards(1, player_deck_cards);

			// TODO: guess oppoennt deck type
			std::string opponent_deck_type = "InnKeeperBasicMage";
			std::vector<Cards::CardId> opponent_deck_cards;
			for (auto const& card_name : Decks::GetDeck(opponent_deck_type)) {
				Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
				opponent_deck_cards.push_back(card_id);
			}
			board_.SetDeckCards(2, opponent_deck_cards);

			board_.Parse(json_board);

			// TODO: we should prepare N samples
			std::mt19937 rand(seed);
			first_unknown_cards_sets_mgr_.Prepare(rand);
			second_unknown_cards_sets_mgr_.Prepare(rand);

			if (need_restart_ai_) {
				controller.reset(new ui::AIController());
			}
			else {
				// TODO: re-use MCTS tree
				return 0;
			}

			return 0;
		}

		state::State GetStartBoard(int seed)
		{
			std::shared_lock<std::shared_mutex> lock(lock_);

			state::State state;
			std::mt19937 rand(seed);

			MyRandomGenerator random(rand());
			MakePlayer(state::kPlayerFirst, state, random, board_.GetFirstPlayer(), first_unknown_cards_sets_mgr_);
			MakePlayer(state::kPlayerSecond, state, random, board_.GetSecondPlayer(), second_unknown_cards_sets_mgr_);
			state.GetMutableCurrentPlayerId().SetFirst(); // AI is helping first player, and should now waiting for an action
			state.SetTurn(board_.GetTurn());

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

		void MakePlayer(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random,
			board::Player const& board_player, board::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
		{
			MakeHero(player, state, board_player.hero);
			MakeHeroPower(player, state, board_player.hero_power);
			MakeDeck(player, state, random, board_player.deck, unknown_cards_sets_mgr);
			MakeHand(player, state, random, board_player.hand, unknown_cards_sets_mgr);
			MakeMinions(player, state, random, board_player.minions);
			// TODO: enchantments
			// TODO: weapon
			// TODO: secrets

			state.GetBoard().Get(player).SetFatigueDamage(board_player.fatigue);
			MakeResource(state.GetBoard().Get(player).GetResource(), board_player.resource);
		}

		void ApplyStatus(state::Cards::CardData & raw_card, board::CharacterStatus const& status)
		{
			raw_card.enchanted_states.charge = status.charge;
			raw_card.taunt = status.taunt;
			raw_card.shielded = status.divine_shield;
			raw_card.enchanted_states.stealth = status.stealth;
			// TODO: forgetful
			raw_card.enchanted_states.freeze_attack = status.freeze;
			raw_card.freezed = status.frozon;
			raw_card.enchanted_states.poisonous = status.poisonous;

			int max_attacks_per_turn = 1;
			if (status.windfury) max_attacks_per_turn = 2;
			raw_card.enchanted_states.max_attacks_per_turn = max_attacks_per_turn;
		}

		void AddMinion(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, board::Minion const& minion, int pos)
		{
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(minion.card_id);
			raw_card.enchanted_states.player = player;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

			raw_card.enchanted_states.max_hp = minion.max_hp;
			raw_card.damaged = minion.damage;
			raw_card.enchanted_states.attack = minion.attack;
			raw_card.num_attacks_this_turn = minion.attack_this_turn;
			// TODO: exhausted (needed?)
			raw_card.silenced = minion.silenced;
			raw_card.enchanted_states.spell_damage = minion.spellpower;
			raw_card.just_played = minion.summoned_this_turn;
			// TODO: enchantments
			ApplyStatus(raw_card, minion.status);

			raw_card.zone = state::kCardZoneNewlyCreated;
			state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
			state.GetZoneChanger<state::kCardTypeMinion, state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZonePlay>(player, pos);
		}

		void MakeMinions(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, board::Minions const& minions)
		{
			int pos = 0;
			for (auto const& minion : minions.minions) {
				AddMinion(player, state, random, minion, pos);
				++pos;
			}
		}

		void MakeResource(state::board::PlayerResource & resource, board::Resource const& board_resource)
		{
			resource.SetCurrent(board_resource.this_turn);
			resource.SetTotal(board_resource.total);
			resource.SetCurrentOverloaded(board_resource.overload);
			resource.SetNextOverload(board_resource.overload_next_turn);
		}

		void MakeHero(state::PlayerIdentifier player, state::State & state, board::Hero const& board_hero)
		{
			// TODO: mark ctor of state::Cards::CardData as private, to make it only constructible from CardDispatcher::CreateInstance
			
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(board_hero.card_id);
			
			raw_card.enchanted_states.player = player;
			assert(raw_card.card_type == state::kCardTypeHero);
			raw_card.zone = state::kCardZoneNewlyCreated;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

			raw_card.enchanted_states.max_hp = board_hero.max_hp;
			raw_card.damaged = board_hero.damage;
			raw_card.armor = board_hero.armor;
			raw_card.enchanted_states.attack = board_hero.attack;
			raw_card.num_attacks_this_turn = board_hero.attack_this_turn;
			ApplyStatus(raw_card, board_hero.status);

			// TODO: enchantments

			state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
			state.GetZoneChanger<state::kCardTypeHero, state::kCardZoneNewlyCreated>(ref)
				.ChangeTo<state::kCardZonePlay>(player);
		}

		void MakeHeroPower(state::PlayerIdentifier player, state::State & state, board::HeroPower const& hero_power)
		{
			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(hero_power.card_id);
			assert(raw_card.card_type == state::kCardTypeHeroPower);
			raw_card.used_this_turn = hero_power.used;

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

		void MakeDeck(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, std::vector<int> entities,
			board::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
		{
			for (int entity_id : entities) {
				Cards::CardId card_id = board_.GetCardId(entity_id, unknown_cards_sets_mgr);
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

		void MakeHand(state::PlayerIdentifier player, state::State & state, state::IRandomGenerator & random, std::vector<int> const& entities,
			board::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
		{
			for (int entity_id : entities) {
				Cards::CardId card_id = board_.GetCardId(entity_id, unknown_cards_sets_mgr);
				AddHandCard(player, state, random, card_id);
			}
		}

	private:
		std::shared_mutex lock_;
		GameEngineLogger & logger_;
		bool need_restart_ai_;
		std::string board_raw_;
		board::Board board_;
		board::UnknownCardsSetsManager first_unknown_cards_sets_mgr_;
		board::UnknownCardsSetsManager second_unknown_cards_sets_mgr_;
	};
}