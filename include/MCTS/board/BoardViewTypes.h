#pragma once

#include <vector>
#include "Cards/id-map.h"
#include "state/Types.h"
#include "Utils/HashCombine.h"

namespace mcts
{
	namespace board
	{
		namespace boardview
		{
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

				bool operator==(Hero const& v) const {
					static_assert(change_id == 1);
					if (attack != v.attack) return false;
					if (hp != v.hp) return false;
					if (max_hp != v.max_hp) return false;
					if (armor != v.armor) return false;
					return true;
				}

				bool operator!=(Hero const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
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

				bool operator==(Crystal const& v) const {
					static_assert(change_id == 1);
					if (current != v.current) return false;
					if (total != v.total) return false;
					if (overload != v.overload) return false;
					if (overload_next_turn != v.overload_next_turn) return false;
					return true;
				}

				bool operator!=(Crystal const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
			};

			struct HeroPower
			{
				Cards::CardId card_id;
				bool usable;

				void Fill(state::Cards::Card const& card) {
					card_id = card.GetCardId();
					usable = card.GetRawData().usable;
				}

				bool operator==(HeroPower const& v) const {
					static_assert(change_id == 1);
					if (card_id != v.card_id) return false;
					if (usable != v.usable) return false;
					return true;
				}

				bool operator!=(HeroPower const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
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

				bool operator==(Weapon const& v) const {
					static_assert(change_id == 1);
					if (equipped != v.equipped) return false;

					if (equipped) {
						if (card_id != v.card_id) return false;
						if (attack != v.attack) return false;
						if (durability != v.durability) return false;
					}

					return true;
				}

				bool operator!=(Weapon const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
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

				bool operator==(Minion const& v) const {
					static_assert(change_id == 1);
					if (card_id != v.card_id) return false;
					if (attack != v.attack) return false;
					if (hp != v.hp) return false;
					if (max_hp != v.max_hp) return false;
					return true;
				}

				bool operator!=(Minion const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
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

				bool operator==(SelfHandCard const& v) const {
					static_assert(change_id == 1);
					if (card_id != v.card_id) return false;
					if (cost != v.cost) return false;
					if (attack != v.attack) return false;
					if (hp != v.hp) return false;
					return true;
				}

				bool operator!=(SelfHandCard const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
			};
			struct OpponentHandCard
			{
				// int hold_from_turn; // TODO
				// bool enchanted; // TODO

				void Fill(state::Cards::Card const& card) {
				}

				bool operator==(OpponentHandCard const& v) const {
					static_assert(change_id == 1);
					return true;
				}

				bool operator!=(OpponentHandCard const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
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

				bool operator==(SelfDeck const& v) const {
					static_assert(change_id == 1);
					if (count != v.count) return false;
					return true;
				}

				bool operator!=(SelfDeck const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
			};

			struct OpponentDeck
			{
				size_t count;

				void Fill(state::board::Deck const& deck) {
					count = deck.Size();
				}

				bool operator==(OpponentDeck const& v) const {
					static_assert(change_id == 1);
					if (count != v.count) return false;
					return true;
				}

				bool operator!=(OpponentDeck const& v) const {
					return !(*this == v);
				}

				static int constexpr change_id = 1;
			};
		}
	}
}

namespace std {
	template <>
	struct hash<mcts::board::boardview::Hero> {
		std::size_t operator()(mcts::board::boardview::Hero const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.attack);
			Utils::HashCombine::hash_combine(result, v.hp);
			Utils::HashCombine::hash_combine(result, v.max_hp);
			Utils::HashCombine::hash_combine(result, v.armor);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::Crystal> {
		std::size_t operator()(mcts::board::boardview::Crystal const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.current);
			Utils::HashCombine::hash_combine(result, v.total);
			Utils::HashCombine::hash_combine(result, v.overload);
			Utils::HashCombine::hash_combine(result, v.overload_next_turn);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::HeroPower> {
		std::size_t operator()(mcts::board::boardview::HeroPower const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.card_id);
			Utils::HashCombine::hash_combine(result, v.usable);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::Weapon> {
		std::size_t operator()(mcts::board::boardview::Weapon const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.equipped);
			if (v.equipped) {
				Utils::HashCombine::hash_combine(result, v.card_id);
				Utils::HashCombine::hash_combine(result, v.attack);
				Utils::HashCombine::hash_combine(result, v.durability);
			}
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::Minion> {
		std::size_t operator()(mcts::board::boardview::Minion const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.card_id);
			Utils::HashCombine::hash_combine(result, v.attack);
			Utils::HashCombine::hash_combine(result, v.hp);
			Utils::HashCombine::hash_combine(result, v.max_hp);
			Utils::HashCombine::hash_combine(result, v.silenced);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::Minions> {
		std::size_t operator()(mcts::board::boardview::Minions const& v) const
		{
			std::size_t result = 0;
			for (auto const& minion : v) {
				Utils::HashCombine::hash_combine(result, minion);
			}
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::SelfHandCard> {
		std::size_t operator()(mcts::board::boardview::SelfHandCard const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.card_id);
			Utils::HashCombine::hash_combine(result, v.cost);
			Utils::HashCombine::hash_combine(result, v.attack);
			Utils::HashCombine::hash_combine(result, v.hp);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::OpponentHandCard> {
		std::size_t operator()(mcts::board::boardview::OpponentHandCard const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::SelfHand> {
		std::size_t operator()(mcts::board::boardview::SelfHand const& v) const
		{
			std::size_t result = 0;
			for (auto const& item : v) {
				Utils::HashCombine::hash_combine(result, item);
			}
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::OpponentHand> {
		std::size_t operator()(mcts::board::boardview::OpponentHand const& v) const
		{
			std::size_t result = 0;
			for (auto const& item : v) {
				Utils::HashCombine::hash_combine(result, item);
			}
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::SelfDeck> {
		std::size_t operator()(mcts::board::boardview::SelfDeck const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.count);
			return result;
		}
	};

	template <>
	struct hash<mcts::board::boardview::OpponentDeck> {
		std::size_t operator()(mcts::board::boardview::OpponentDeck const& v) const
		{
			static_assert(std::decay_t<decltype(v)>::change_id == 1);
			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.count);
			return result;
		}
	};
}