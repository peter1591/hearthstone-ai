#pragma once

#include <assert.h>
#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			void EnchantmentHelper::Update()
			{
				state::Cards::EnchantmentAuxData & data = card_.GetMutableEnchantmentAuxDataGetter().Get();

				if (!data.need_update) return;

				state::Cards::EnchantableStates states = data.origin_states;
				data.enchantments.ApplyAll(states);

				auto card_manipulator = Manipulate(state_, flow_context_).Card(card_ref_);

				// update states field by field
				bool update_zone = false;
				if (states.player != data.origin_states.player) {
					switch (card_.GetCardType()) {
					case state::kCardTypeMinion:
						ChangeMinionPlayer(states.player);
						break;
					default:
						throw std::exception("unsupported enchantment type");
					}
				}

				if (states.cost != data.origin_states.cost) {
					card_manipulator.Cost(states.cost);
					assert(card_.GetCost() == states.cost);
				}
				if (states.attack != data.origin_states.attack) {
					card_manipulator.Attack(states.attack);
					assert(card_.GetAttack() == states.attack);
				}
				if (states.max_hp != data.origin_states.max_hp) {
					card_manipulator.MaxHP(states.max_hp);
					assert(card_.GetMaxHP() == states.max_hp);
				}
			}

			void EnchantmentHelper::ChangeMinionPlayer(state::PlayerIdentifier player)
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