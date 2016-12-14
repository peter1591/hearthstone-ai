#pragma once

#include <assert.h>
#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "Cards/Database.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void EnchantmentHelper::Update()
			{
				state::Cards::EnchantmentAuxData & data = card_.GetMutableEnchantmentAuxDataGetter().Get();

				if (!data.need_update) return;

				const state::Cards::EnchantableStates & origin_states = data.origin_states;
				state::Cards::EnchantableStates new_states = origin_states;
				data.enchantments.ApplyAll(new_states);

				auto card_manipulator = Manipulate(state_, flow_context_).Card(card_ref_);

				static_assert(state::Cards::EnchantableStates::kFieldChangeId == 1, "enchantable fields changed");

				// update states field by field
				bool update_zone = false;
				if (new_states.player != origin_states.player) {
					switch (card_.GetCardType()) {
					case state::kCardTypeMinion:
						ChangeMinionPlayer(new_states.player);
						break;
					default:
						throw std::exception("unsupported enchantment type");
					}
				}

				if (new_states.cost != origin_states.cost) {
					card_manipulator.Cost(new_states.cost);
					assert(card_.GetCost() == new_states.cost);
				}
				if (new_states.attack != origin_states.attack) {
					card_manipulator.Attack(new_states.attack);
					assert(card_.GetAttack() == new_states.attack);
				}
				if (new_states.max_hp != origin_states.max_hp) {
					card_manipulator.MaxHP(new_states.max_hp);
					assert(card_.GetMaxHP() == new_states.max_hp);
				}

				data.need_update = false;
			}

			inline void EnchantmentHelper::ChangeMinionPlayer(state::PlayerIdentifier player)
			{
				assert(card_.GetZone() == state::kCardZonePlay);

				if (state_.board.Get(player).minions_.Full()) {
					Manipulate(state_, flow_context_).Minion(card_ref_).Zone().ChangeTo<state::kCardZoneGraveyard>(player);
					assert(card_.GetZone() == state::kCardZoneGraveyard);
					assert(card_.GetPlayerIdentifier() == player);
				}
				else {
					int location = (int)state_.board.Get(player).minions_.Size();

					Manipulate(state_, flow_context_).Minion(card_ref_).Zone().ChangeTo<state::kCardZonePlay>(player, location);
					assert(card_.GetZone() == state::kCardZonePlay);
					assert(card_.GetPlayerIdentifier() == player);
				}
			}
		}
	}
}