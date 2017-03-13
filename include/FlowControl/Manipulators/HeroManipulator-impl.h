#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroManipulator::DrawCard()
		{
			state::board::Player & player = state_.GetBoard().Get(player_id_);
			assert(player.hero_ref_ == card_ref_);

			if (player.deck_.Empty())
			{
				player.IncreaseFatigueDamage();
				int damage = player.GetFatigueDamage();
				this->Damage(damage);
				return;
			}

			state::CardRef card_ref = player.deck_.GetLast();

			if (player.hand_.Full()) {
				state_.GetZoneChanger(flow_context_, card_ref).WithZone<state::kCardZoneDeck>()
					.ChangeTo<state::kCardZoneGraveyard>(player_id_);
			}
			else {
				state_.GetZoneChanger(flow_context_, card_ref).WithZone<state::kCardZoneDeck>()
					.ChangeTo<state::kCardZoneHand>(player_id_);
			}

			// TODO: trigger on-draw event (parameter: card_ref)
		}
		
		inline void HeroManipulator::DestroyWeapon()
		{
			if (!card_.GetRawData().weapon_ref.IsValid()) return;

			if (!flow_context_.destroyed_weapon_.IsValid()) {
				flow_context_.destroyed_weapon_ = card_.GetRawData().weapon_ref;
			}
			card_.ClearWeapon();

			assert(!card_.GetRawData().weapon_ref.IsValid());
		}

		template <state::CardZone KnownZone>
		inline void HeroManipulator::EquipWeapon(state::CardRef weapon_ref)
		{
			DestroyWeapon();

			card_.SetWeapon(weapon_ref);

			state_.GetZoneChanger(flow_context_, weapon_ref)
				.WithType<state::kCardTypeWeapon>()
				.WithZone<KnownZone>()
				.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId());
		}
	}
}