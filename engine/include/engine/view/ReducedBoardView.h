#pragma once

#include "state/Types.h"
#include "engine/view/BoardRefView.h"
#include "engine/view/ReducedBoardViewTypes.h"
#include "Utils/HashCombine.h"

namespace engine
{
	namespace view
	{
		// Reduced board view which is designed to determine if an identical game state is reached
		// via different action orderings, and is also designed to be storable and hasable
		class ReducedBoardView
		{
			friend std::hash<ReducedBoardView>;

		public:
			template <state::PlayerSide Side>
			ReducedBoardView(engine::view::BoardRefView<Side> const& board);

			ReducedBoardView(ReducedBoardView const&) = default;
			ReducedBoardView(ReducedBoardView &&) = default;

			ReducedBoardView & operator=(ReducedBoardView const&) = default;
			ReducedBoardView & operator=(ReducedBoardView &&) = default;

			bool operator==(ReducedBoardView const& rhs) const {
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

			bool operator!=(ReducedBoardView const& rhs) const {
				return !(*this == rhs);
			}

		public:
			int GetTurn() const { return turn_; }
			state::PlayerSide GetSide() const { return side_; }
			
			reduced_board_view::SelfHero const& GetSelfHero() const { return self_hero_; }
			reduced_board_view::Crystal const& GetSelfCrystal() const { return self_crystal_; }
			reduced_board_view::HeroPower const& GetHeroPower() const { return self_hero_power_; }
			reduced_board_view::Weapon const& GetSelfWeapon() const { return self_weapon_; }
			reduced_board_view::SelfMinions const& GetSelfMinions() const { return self_minions_; }
			reduced_board_view::SelfHand const& GetSelfHand() const { return self_hand_; }
			reduced_board_view::SelfDeck const& GetSelfDeck() const { return self_deck_; }

			reduced_board_view::Hero const& GetOpponentHero() const { return opponent_hero_; }
			reduced_board_view::Crystal const& GetOpponentCrystal() const { return opponent_crystal_; }
			reduced_board_view::HeroPower const& GetOpponentHeroPower() const { return opponent_hero_power_; }
			reduced_board_view::Minions const& GetOpponentMinions() const { return opponent_minions_; }
			reduced_board_view::Weapon const& GetOpponentWeapon() const { return opponent_weapon_; }
			reduced_board_view::OpponentHand const& GetOpponentHand() const { return opponent_hand_; }
			reduced_board_view::OpponentDeck const& GetOpponentDeck() const { return opponent_deck_; }

		private:
			static int constexpr change_id = 3;

			int turn_;
			state::PlayerSide side_;

			reduced_board_view::SelfHero self_hero_;
			reduced_board_view::Crystal self_crystal_;
			reduced_board_view::HeroPower self_hero_power_;
			reduced_board_view::Weapon self_weapon_;
			reduced_board_view::SelfMinions self_minions_;
			reduced_board_view::SelfHand self_hand_;
			reduced_board_view::SelfDeck self_deck_;

			reduced_board_view::Hero opponent_hero_;
			reduced_board_view::Crystal opponent_crystal_;
			reduced_board_view::HeroPower opponent_hero_power_;
			reduced_board_view::Weapon opponent_weapon_;
			reduced_board_view::Minions opponent_minions_;
			reduced_board_view::OpponentHand opponent_hand_;
			reduced_board_view::OpponentDeck opponent_deck_;
		};
	}
}

namespace std {
	template <>
	struct hash<engine::view::ReducedBoardView> {
		std::size_t operator()(engine::view::ReducedBoardView const& v) const
		{
			static_assert(engine::view::ReducedBoardView::change_id == 3);

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