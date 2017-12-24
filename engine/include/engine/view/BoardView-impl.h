#pragma once

#include "engine/view/BoardView.h"

namespace engine
{
	namespace view
	{
		template <state::PlayerSide Side>
		inline BoardView::BoardView(engine::view::BoardRefView<Side> const& board) :
			turn_(board.GetTurn()), side_(Side),
			self_hero_(), self_crystal_(), self_hero_power_(), self_weapon_(),
			self_minions_(), self_hand_(), self_deck_(),
			opponent_hero_(), opponent_crystal_(), opponent_hero_power_(),
			opponent_weapon_(), opponent_minions_(), opponent_hand_(), opponent_deck_()
		{
			{
				self_hero_.Fill(board.GetSelfHero(), board.IsHeroAttackable(Side));

				self_crystal_.Fill(board.GetPlayerResource(Side));

				self_hero_power_.Fill(board.GetHeroPower(Side));

				self_weapon_.Invalidate();
				board.GetWeapon(Side, [&](state::Cards::Card const& card) {
					self_weapon_.Fill(card);
				});

				board.ForEachMinion(Side, [&](state::Cards::Card const& card, bool attackable) {
					self_minions_.emplace_back(card, attackable);
					return true;
				});

				board.ForEachSelfHandCard([&](state::Cards::Card const& card) {
					self_hand_.emplace_back(card);
					return true;
				});

				self_deck_.Fill(board.GetDeckCardCount(Side));
			}

			{
				state::PlayerSide opponent_side = state::PlayerIdentifier(Side).Opposite().GetSide();

				opponent_hero_.Fill(board.GetOpponentHero());

				opponent_crystal_.Fill(board.GetPlayerResource(opponent_side));

				opponent_hero_power_.Fill(board.GetHeroPower(opponent_side));

				opponent_weapon_.Invalidate();
				board.GetWeapon(opponent_side, [&](state::Cards::Card const& card) {
					opponent_weapon_.Fill(card);
				});

				board.ForEachMinion(opponent_side, [&](state::Cards::Card const& card, bool attackable) {
					(void)attackable;
					opponent_minions_.emplace_back(card);
					return true;
				});

				board.ForEachOpponentHandCard([&]() {
					opponent_hand_.emplace_back();
					return true;
				});

				opponent_deck_.Fill(board.GetDeckCardCount(opponent_side));
			}
		}
	}
}