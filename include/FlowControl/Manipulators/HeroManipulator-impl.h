#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroManipulator::DrawCard()
		{
			state::board::Player & player = state_.board.Get(player_id_);
			assert(player.hero_ref_ == card_ref_);

			if (player.deck_.Empty())
			{
				int damage = ++player.fatigue_damage_;
				this->Damage(damage);
				return;
			}

			state::CardRef card_ref = player.deck_.GetLast();

			if (player.hand_.Full()) {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().WithZone<state::kCardZoneDeck>()
					.ChangeTo<state::kCardZoneGraveyard>(player_id_);
			}
			else {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().WithZone<state::kCardZoneDeck>()
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

			Manipulate(state_, flow_context_).Weapon(weapon_ref).Zone().WithZone<KnownZone>()
				.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId());
		}
	}
}