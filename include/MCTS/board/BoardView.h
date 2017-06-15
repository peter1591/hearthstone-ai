#pragma once

#include "state/Types.h"
#include "MCTS/board/Board.h"
#include "MCTS/board/BoardViewTypes.h"

namespace mcts
{
	namespace board
	{
		template<state::PlayerSide Side>
		class BoardView
		{
		public:
			BoardView(Board const& board);
			BoardView(BoardView const&) = default;
			BoardView(BoardView &&) = default;

			BoardView & operator=(BoardView const&) = default;
			BoardView & operator=(BoardView &&) = default;

		public:
			int GetTurn() const { return turn_; }
			
			Hero const& GetSelfHero() const { return self_hero_; }
			Crystal const& GetSelfCrystal() const { return self_crystal_; }
			HeroPower const& GetHeroPower() const { return self_hero_power_; }
			Weapon const& GetSelfWeapon() const { return self_weapon_; }
			SelfHand const& GetSelfHand() const { return self_hand_; }
			SelfDeck const& GetSelfDeck() const { return self_deck_; }

			Hero const& GetOpponentHero() const { return opponent_hero_; }
			Crystal const& GetOpponentCrystal() const { return opponent_crystal_; }
			HeroPower const& GetOpponentHeroPower() const { return opponent_hero_power_; }
			Weapon const& GetOpponentWeapon() const { return opponent_weapon_; }
			OpponentHand const& GetOpponentHand() const { return opponent_hand_; }
			OpponentDeck const& GetOpponentDeck() const { return opponent_deck_; }

		private:
			int turn_;

			Hero self_hero_;
			Crystal self_crystal_;
			HeroPower self_hero_power_;
			Weapon self_weapon_;
			SelfHand self_hand_;
			SelfDeck self_deck_;

			Hero opponent_hero_;
			Crystal opponent_crystal_;
			HeroPower opponent_hero_power_;
			Weapon opponent_weapon_;
			OpponentHand opponent_hand_;
			OpponentDeck opponent_deck_;
		};
	}
}