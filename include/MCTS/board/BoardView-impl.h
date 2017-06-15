#pragma once

#include "MCTS/board/Board.h"
#include "MCTS/board/BoardView.h"

namespace mcts
{
	namespace board
	{
		template <state::PlayerSide Side>
		inline BoardView<Side>::BoardView(Board const& board)
		{
			state::State const& state = board.GetState();
			
			turn_ = state.GetTurn();

			{
				state::board::Player const& self_player = state.GetBoard().Get(Side);

				state::CardRef hero_ref = self_player.GetHeroRef();
				assert(hero_ref.IsValid());
				self_hero_.Fill(state.GetCard(hero_ref));

				self_crystal_.Fill(self_player);

				state::CardRef hero_power_ref = self_player.GetHeroPowerRef();
				assert(hero_power_ref.IsValid());
				self_hero_power_.Fill(state.GetCard(hero_power_ref));

				state::CardRef weapon_ref = self_player.GetWeaponRef();
				if (weapon_ref.IsValid()) self_weapon_.Fill(state.GetCard(self_player.GetWeaponRef()));
				else self_weapon_.Invalidate();

				self_player.hand_.ForEach([&](state::CardRef card_ref) {
					SelfHandCard item;
					item.Fill(state.GetCard(card_ref));
					self_hand_.push_back(std::move(item));
				});

				self_deck_.Fill(self_player.deck_);
			}

			{
				state::PlayerIdentifier opponent_side = state::PlayerIdentifier(Side).Opposite();
				state::board::Player const& opponent_player = state.GetBoard().Get(opponent_side);

				state::CardRef hero_ref = opponent_player.GetHeroRef();
				assert(hero_ref.IsValid());
				opponent_hero_.Fill(state.GetCard(hero_ref));

				opponent_crystal_.Fill(self_player);

				state::CardRef hero_power_ref = opponent_player.GetHeroPowerRef();
				assert(hero_power_ref.IsValid());
				opponent_hero_power_.Fill(state.GetCard(hero_power_ref));

				state::CardRef weapon_ref = opponent_player.GetWeaponRef();
				if (weapon_ref.IsValid()) opponent_weapon_.Fill(state.GetCard(opponent_player.GetWeaponRef()));
				else opponent_weapon_.Invalidate();

				opponent_player.hand_.ForEach([&](state::CardRef card_ref) {
					SelfHandCard item;
					item.Fill(state.GetCard(card_ref));
					self_hand_.push_back(std::move(item));
				});

				opponent_deck_.Fill(opponent_player.deck_);
			}
		}
	}
}