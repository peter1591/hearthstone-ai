#pragma once

#include "state/Types.h"
#include "engine/PlayerStateView.h"
#include "MCTS/board/BoardViewTypes.h"
#include "Utils/HashCombine.h"

namespace mcts
{
	namespace board
	{
		class BoardView
		{
			friend std::hash<BoardView>;

		public:
			template <state::PlayerSide Side>
			BoardView(engine::PlayerStateView<Side> const& board);

			BoardView(BoardView const&) = default;
			BoardView(BoardView &&) = default;

			BoardView & operator=(BoardView const&) = default;
			BoardView & operator=(BoardView &&) = default;

			bool operator==(BoardView const& rhs) const {
				static_assert(change_id == 3);
				if (turn_ != rhs.turn_) return false;
				if (side_ != rhs.side_) return false;

				if (self_hero_ != rhs.self_hero_) return false;
				if (self_crystal_ != rhs.self_crystal_) return false;
				if (self_hero_power_ != rhs.self_hero_power_) return false;
				if (self_minions_ != rhs.self_minions_) return false;
				if (self_weapon_ != rhs.self_weapon_) return false;
				if (self_hand_ != rhs.self_hand_) return false;
				if (self_deck_ != rhs.self_deck_) return false;

				if (opponent_hero_ != rhs.opponent_hero_) return false;
				if (opponent_crystal_ != rhs.opponent_crystal_) return false;
				if (opponent_hero_power_ != rhs.opponent_hero_power_) return false;
				if (opponent_minions_ != rhs.opponent_minions_) return false;
				if (opponent_weapon_ != rhs.opponent_weapon_) return false;
				if (opponent_hand_ != rhs.opponent_hand_) return false;
				if (opponent_deck_ != rhs.opponent_deck_) return false;

				return true;
			}

			bool operator!=(BoardView const& rhs) const {
				return !(*this == rhs);
			}

		public:
			int GetTurn() const { return turn_; }
			state::PlayerSide GetSide() const { return side_; }
			
			boardview::SelfHero const& GetSelfHero() const { return self_hero_; }
			boardview::Crystal const& GetSelfCrystal() const { return self_crystal_; }
			boardview::HeroPower const& GetHeroPower() const { return self_hero_power_; }
			boardview::Weapon const& GetSelfWeapon() const { return self_weapon_; }
			boardview::SelfMinions const& GetSelfMinions() const { return self_minions_; }
			boardview::SelfHand const& GetSelfHand() const { return self_hand_; }
			boardview::SelfDeck const& GetSelfDeck() const { return self_deck_; }

			boardview::Hero const& GetOpponentHero() const { return opponent_hero_; }
			boardview::Crystal const& GetOpponentCrystal() const { return opponent_crystal_; }
			boardview::HeroPower const& GetOpponentHeroPower() const { return opponent_hero_power_; }
			boardview::Minions const& GetOpponentMinions() const { return opponent_minions_; }
			boardview::Weapon const& GetOpponentWeapon() const { return opponent_weapon_; }
			boardview::OpponentHand const& GetOpponentHand() const { return opponent_hand_; }
			boardview::OpponentDeck const& GetOpponentDeck() const { return opponent_deck_; }

		private:
			static int constexpr change_id = 3;

			int turn_;
			state::PlayerSide side_;

			boardview::SelfHero self_hero_;
			boardview::Crystal self_crystal_;
			boardview::HeroPower self_hero_power_;
			boardview::Weapon self_weapon_;
			boardview::SelfMinions self_minions_;
			boardview::SelfHand self_hand_;
			boardview::SelfDeck self_deck_;

			boardview::Hero opponent_hero_;
			boardview::Crystal opponent_crystal_;
			boardview::HeroPower opponent_hero_power_;
			boardview::Weapon opponent_weapon_;
			boardview::Minions opponent_minions_;
			boardview::OpponentHand opponent_hand_;
			boardview::OpponentDeck opponent_deck_;
		};
	}
}

namespace std {
	template <>
	struct hash<mcts::board::BoardView> {
		std::size_t operator()(mcts::board::BoardView const& v) const
		{
			static_assert(mcts::board::BoardView::change_id == 3);

			std::size_t result = 0;
			Utils::HashCombine::hash_combine(result, v.turn_);
			Utils::HashCombine::hash_combine(result, (int)v.side_);

			Utils::HashCombine::hash_combine(result, v.self_hero_);
			Utils::HashCombine::hash_combine(result, v.self_crystal_);
			Utils::HashCombine::hash_combine(result, v.self_hero_power_);
			Utils::HashCombine::hash_combine(result, v.self_minions_);
			Utils::HashCombine::hash_combine(result, v.self_weapon_);
			Utils::HashCombine::hash_combine(result, v.self_hand_);
			Utils::HashCombine::hash_combine(result, v.self_deck_);

			Utils::HashCombine::hash_combine(result, v.opponent_hero_);
			Utils::HashCombine::hash_combine(result, v.opponent_crystal_);
			Utils::HashCombine::hash_combine(result, v.opponent_hero_power_);
			Utils::HashCombine::hash_combine(result, v.opponent_minions_);
			Utils::HashCombine::hash_combine(result, v.opponent_weapon_);
			Utils::HashCombine::hash_combine(result, v.opponent_hand_);
			Utils::HashCombine::hash_combine(result, v.opponent_deck_);

			return result;
		}
	};
}