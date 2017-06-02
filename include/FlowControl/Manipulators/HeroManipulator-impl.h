#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline state::CardRef PlayerManipulator::DrawCard(Cards::CardId * drawn_card_id)
		{
			state::board::Player & player = state_.GetBoard().Get(player_);

			if (player.deck_.Empty())
			{
				state::CardRef hero_ref = state_.GetBoard().Get(player_).GetHeroRef();
				player.IncreaseFatigueDamage();
				int damage = player.GetFatigueDamage();
				HeroManipulator(state_, flow_context_, hero_ref).Damage(hero_ref, damage);
				if (drawn_card_id) *drawn_card_id = (Cards::CardId)-1;
				return state::CardRef();
			}

			Cards::CardId card_id = player.deck_.GetLast();
			if (drawn_card_id) *drawn_card_id = (Cards::CardId)card_id;

			player.deck_.RemoveLast();

			return AddHandCard(card_id);

			// Note: On-draw effects are only activated through successful card draw.
			// Overdrawing a card with an on-draw effect (attempting to draw it when the player already has a full hand) will not activate its on-draw effect.
			// TODO: trigger on-draw event (parameter: card_ref)
		}

		inline state::CardRef PlayerManipulator::AddHandCard(Cards::CardId card_id)
		{
			state::board::Player & player = state_.GetBoard().Get(player_);

			if (player.hand_.Full()) return state::CardRef();

			auto ref = BoardManipulator(state_, flow_context_).AddCardById(card_id, player_);
			state_.GetZoneChanger<state::kCardZoneNewlyCreated>(FlowControl::Manipulate(state_, flow_context_), ref)
				.ChangeTo<state::kCardZoneHand>(player_);
			return ref;
		}

		inline void PlayerManipulator::DiscardHandCard(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetPlayerIdentifier() == player_);
			assert(state_.GetCard(card_ref).GetZone() == state::kCardZoneHand);

			// TODO: fire event: DiscardHandCard
			state_.GetZoneChanger<state::kCardZoneHand>(FlowControl::Manipulate(state_, flow_context_), card_ref)
				.ChangeTo<state::kCardZoneSetASide>(player_);
		}

		inline void PlayerManipulator::EquipWeapon(Cards::CardId card_id)
		{
			state::CardRef weapon_ref = BoardManipulator(state_, flow_context_).AddCardById(card_id, player_);
			return EquipWeapon<state::kCardZoneNewlyCreated>(weapon_ref);
		}

		template <state::CardZone KnownZone>
		inline void PlayerManipulator::EquipWeapon(state::CardRef weapon_ref)
		{
			DestroyWeapon();

			state_.GetZoneChanger<state::kCardTypeWeapon, KnownZone>(Manipulate(state_, flow_context_), weapon_ref)
				.ChangeTo<state::kCardZonePlay>(player_);
		}
		
		inline void PlayerManipulator::DestroyWeapon()
		{
			state::CardRef weapon_ref = state_.GetBoard().Get(player_).GetWeaponRef();
			if (!weapon_ref.IsValid()) return;

			if (!flow_context_.GetDestroyedWeapon().IsValid()) {
				flow_context_.SetDestroyedWeapon(weapon_ref);
			}

			state_.GetZoneChanger<state::kCardTypeWeapon, state::kCardZonePlay>(Manipulate(state_, flow_context_), weapon_ref)
				.ChangeTo<state::kCardZoneGraveyard>(state_.GetCurrentPlayerId());

			assert(state_.GetBoard().Get(player_).GetWeaponRef().IsValid() == false);
		}

		inline void HeroManipulator::GainArmor(int amount)
		{
			GetCard().SetArmor(GetCard().GetArmor() + amount);

			// TODO: trigger events
		}

		inline state::CardRef HeroManipulator::Transform(Cards::CardId id)
		{
			assert(GetCard().GetCardType() == state::kCardTypeHero);
			assert(GetCard().GetZone() == state::kCardZonePlay);

			state::CardRef new_card_ref = BoardManipulator(state_, flow_context_).AddCardById(id, GetCard().GetPlayerIdentifier());
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeHero>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			return new_card_ref;
		}

		inline void PlayerManipulator::ReplaceHeroPower(Cards::CardId id)
		{
			// This interface is not located in HeroManipulator because:
			//    The hero-power-ref is recorded in Board, and its automatically maintained by state_.GetZoneChanger()

			state::CardRef old_ref = state_.GetBoard().Get(player_).GetHeroPowerRef();
			state::CardRef new_ref = BoardManipulator(state_, flow_context_).AddCardById(id, player_);

			assert(state_.GetCard(new_ref).GetRawData().usable == true);

			state_.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZonePlay>
				(Manipulate(state_, flow_context_), old_ref)
				.ReplaceBy(new_ref);
		}
	}
}