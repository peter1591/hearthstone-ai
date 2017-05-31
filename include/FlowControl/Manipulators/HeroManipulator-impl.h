#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline state::CardRef HeroManipulator::DrawCard(Cards::CardId * drawn_card_id)
		{
			state::board::Player & player = state_.GetBoard().Get(player_id_);
			assert(player.GetHeroRef() == card_ref_);

			if (player.deck_.Empty())
			{
				player.IncreaseFatigueDamage();
				int damage = player.GetFatigueDamage();
				this->Damage(card_ref_, damage);
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

		inline state::CardRef HeroManipulator::AddHandCard(Cards::CardId card_id)
		{
			state::board::Player & player = state_.GetBoard().Get(player_id_);
			assert(player.GetHeroRef() == card_ref_);

			if (player.hand_.Full()) return state::CardRef();

			state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(card_id);
			raw_card.enchanted_states.player = player_id_;
			raw_card.zone = state::kCardZoneNewlyCreated;
			raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

			auto ref = state_.AddCard(state::Cards::Card(raw_card));
			state_.GetZoneChanger<state::kCardZoneNewlyCreated>(FlowControl::Manipulate(state_, flow_context_), ref)
				.ChangeTo<state::kCardZoneHand>(player_id_);
			return ref;
		}

		inline void HeroManipulator::DiscardHandCard(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetPlayerIdentifier() == GetCard().GetPlayerIdentifier());
			assert(state_.GetCard(card_ref).GetZone() == state::kCardZoneHand);

			// TODO: fire event: DiscardHandCard
			state_.GetZoneChanger<state::kCardZoneHand>(FlowControl::Manipulate(state_, flow_context_), card_ref)
				.ChangeTo<state::kCardZoneSetASide>(state_.GetCard(card_ref).GetPlayerIdentifier());
		}
		
		inline void HeroManipulator::DestroyWeapon()
		{
			state::CardRef weapon_ref = state_.GetBoard().Get(GetCard().GetPlayerIdentifier()).GetWeaponRef();
			if (!weapon_ref.IsValid()) return;

			if (!flow_context_.GetDestroyedWeapon().IsValid()) {
				flow_context_.SetDestroyedWeapon(weapon_ref);
			}

			state_.GetZoneChanger<state::kCardTypeWeapon, state::kCardZonePlay>(Manipulate(state_, flow_context_), weapon_ref)
				.ChangeTo<state::kCardZoneGraveyard>(state_.GetCurrentPlayerId());

			assert(state_.GetBoard().Get(GetCard().GetPlayerIdentifier()).GetWeaponRef().IsValid() == false);
		}

		inline void HeroManipulator::GainArmor(int amount)
		{
			GetCard().SetArmor(GetCard().GetArmor() + amount);

			// TODO: trigger events
		}

		inline void HeroManipulator::EquipWeapon(Cards::CardId card_id)
		{
			state::CardRef weapon_ref = BoardManipulator(state_, flow_context_).AddCardById(card_id, player_id_);
			return EquipWeapon<state::kCardZoneNewlyCreated>(weapon_ref);
		}

		template <state::CardZone KnownZone>
		inline void HeroManipulator::EquipWeapon(state::CardRef weapon_ref)
		{
			DestroyWeapon();

			state_.GetZoneChanger<state::kCardTypeWeapon, KnownZone>(Manipulate(state_, flow_context_), weapon_ref)
				.ChangeTo<state::kCardZonePlay>(GetCard().GetPlayerIdentifier());
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
	}
}