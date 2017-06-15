#pragma once

#include <vector>
#include "Cards/id-map.h"
#include "state/Types.h"

namespace mcts
{
	namespace board
	{
		// TODO: Need to clarify when is this BoardView data is used
		// If it is only used to distinguish the outcome of the children of a step?

		enum Side
		{
			kSideSelf,
			kSideOpponent
		};

		struct Hero
		{
			// TODO: playerClass
			int attack;
			int hp;
			int max_hp;
			int armor;

			void Fill(state::Cards::Card const& hero) {
				attack = hero.GetAttack();
				hp = hero.GetHP();
				max_hp = hero.GetMaxHP();
				armor = hero.GetArmor();
			}
		};

		struct Crystal
		{
			int current;
			int total;
			int overload;
			int overload_next_turn;

			void Fill(state::board::Player const& player) {
				current = player.GetResource().GetCurrent();
				total = player.GetResource().GetTotal();
				overload = player.GetResource().GetCurrentOverloaded();
				overload_next_turn = player.GetResource().GetNextOverload();
			}
		};

		struct HeroPower
		{
			Cards::CardId card_id;
			bool usable;

			void Fill(state::Cards::Card const& card) {
				card_id = card.GetCardId();
				usable = card.GetRawData().usable;
			}
		};

		struct Weapon
		{
			bool equipped;
			Cards::CardId card_id;
			int attack;
			int durability;

			void Fill(state::Cards::Card const& card) {
				equipped = true;
				card_id = card.GetCardId();
				attack = card.GetAttack();
				durability = card.GetHP();
			}

			void Invalidate() { equipped = false; }
		};

		struct Minion
		{
			Cards::CardId card_id;
			int attack;
			int hp;
			int max_hp;

			bool silenced;

			void Fill(state::Cards::Card const& card) {
				card_id = card.GetCardId();
				attack = card.GetAttack();
				hp = card.GetHP();
				max_hp = card.GetMaxHP();
				silenced = card.IsSilenced();
			}
		};

		using Minions = std::vector<Minion>;

		struct SelfHandCard
		{
			Cards::CardId card_id;
			int cost;
			int attack;
			int hp;

			void Fill(state::Cards::Card const& card) {
				card_id = card.GetCardId();
				cost = card.GetCost();
				attack = card.GetAttack();
				hp = card.GetHP();
			}
		};
		struct OpponentHandCard
		{
			// int hold_from_turn; // TODO
			// bool enchanted; // TODO

			void Fill(state::Cards::Card const& card) {
			}
		};

		using SelfHand = std::vector<SelfHandCard>;
		using OpponentHand = std::vector<OpponentHandCard>;

		struct SelfDeck
		{
			//std::unordered_set<Cards::CardId> cards; // TODO: do we really need this in board view?
			int count;

			void Fill(state::board::Deck const& deck) {
				count = deck.Size();
			}
		};

		struct OpponentDeck
		{
			size_t count;

			void Fill(state::board::Deck const& deck) {
				count = deck.Size();
			}
		};
	}
}