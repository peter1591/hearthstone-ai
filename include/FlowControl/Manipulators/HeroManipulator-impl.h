#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroManipulator::DrawCard()
		{
			state::board::Player & player = state_.GetBoard().Get(player_id_);
			assert(player.GetHeroRef() == card_ref_);

			if (player.deck_.Empty())
			{
				player.IncreaseFatigueDamage();
				int damage = player.GetFatigueDamage();
				this->Damage(damage);
				return;
			}

			state::CardRef card_ref = player.deck_.GetLast();

			if (player.hand_.Full()) {
				state_.GetZoneChanger<state::kCardZoneDeck>(flow_context_.random_, card_ref)
					.ChangeTo<state::kCardZoneGraveyard>(player_id_);
			}
			else {
				state_.GetZoneChanger<state::kCardZoneDeck>(flow_context_.random_, card_ref)
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

			state_.GetZoneChanger<state::kCardTypeWeapon, KnownZone>(flow_context_.random_, weapon_ref)
				.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId());
		}
	}
}