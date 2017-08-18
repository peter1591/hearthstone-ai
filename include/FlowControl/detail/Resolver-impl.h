#pragma once

#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"
#include "FlowControl/deathrattle/Context.h"
#include "FlowControl/detail/Resolver.h"

namespace FlowControl
{
	namespace detail
	{
		inline bool Resolver::Resolve(state::State & state, FlowContext & flow_context)
		{
			while (true) {
				auto first_minions_change_id = state.GetBoard().GetFirst().minions_.GetChangeId();
				auto second_minions_change_id = state.GetBoard().GetSecond().minions_.GetChangeId();

				UpdateAura(state, flow_context); // update aura first, since aura will add/remove enchantments on others
				UpdateEnchantments(state, flow_context);

				do {
					CreateDeaths(state, flow_context);
					if (!RemoveDeaths(state, flow_context)) return false;
				} while (!deaths_.empty());

				bool done = true;

				// If any minions change side (due to enchantment), re-resolve again
				auto new_first_minions_change_id = state.GetBoard().GetFirst().minions_.GetChangeId();
				auto new_second_minions_change_id = state.GetBoard().GetSecond().minions_.GetChangeId();
				if (new_first_minions_change_id != first_minions_change_id ||
					new_second_minions_change_id != second_minions_change_id) {
					done = false;
				}
				if (!done) continue;

				break;
			}

			return true;
		}

		inline void Resolver::CreateDeaths(state::State & state, FlowContext & flow_context)
		{
			deaths_.clear();

			if (flow_context.GetDestroyedWeapon().IsValid()) {
				flow_context.AddDeadEntryHint(state, flow_context.GetDestroyedWeapon());
				flow_context.ClearDestroyedWeapon();
			}

			flow_context.ForEachDeadEntryHint([&state, this](auto const& item) {
				state::CardRef ref = item.second;
				const state::Cards::Card & card = state.GetCardsManager().Get(ref);

				if (card.GetZone() != state::kCardZonePlay) return;
				if (!card.GetRawData().pending_destroy) {
					if (card.GetHP() > 0) return;
				}

				deaths_.insert(ref);
			});

			flow_context.ClearDeadEntryHint();
		}

		inline bool Resolver::RemoveDeaths(state::State & state, FlowContext & flow_context)
		{
			auto first_it = deaths_.find(state.GetBoard().GetFirst().GetHeroRef());
			bool first_dead = first_it != deaths_.end();

			auto second_it = deaths_.find(state.GetBoard().GetSecond().GetHeroRef());
			bool second_dead = second_it != deaths_.end();

			if (first_dead && second_dead) return SetResult(flow_context, kResultDraw);
			if (first_dead) return SetResult(flow_context, kResultSecondPlayerWin);
			if (second_dead) return SetResult(flow_context, kResultFirstPlayerWin);

			// process deaths by order of play
			std::multimap<int, std::function<void()>> ordered_deaths;

			for (auto ref : deaths_) {
				state::Cards::Card & card = state.GetMutableCard(ref);

				state::PlayerIdentifier player = card.GetPlayerIdentifier();
				state::CardZone zone = card.GetZone();
				int zone_pos = card.GetZonePosition();
				int attack = card.GetAttack();

				ordered_deaths.insert(std::make_pair(card.GetPlayOrder(),
					[ref, player, zone, zone_pos, attack, &state, &flow_context]() {
					state.GetMutableCard(ref).GetMutableDeathrattleHandler().TriggerAll(
						FlowControl::deathrattle::context::Deathrattle{
						FlowControl::Manipulate(state, flow_context), ref, player, zone, zone_pos, attack });

					if (state.GetCard(ref).GetCardType() == state::kCardTypeMinion) {
						state.TriggerEvent<state::Events::EventTypes::AfterMinionDied>(state::Events::EventTypes::AfterMinionDied::Context{
							FlowControl::Manipulate(state, flow_context), ref, player
						});
					}
				}));
			}

			for (auto ref : deaths_) {
				state::Cards::Card & card = state.GetMutableCard(ref);
				state.GetZoneChanger(ref)
					.ChangeTo<state::kCardZoneGraveyard>(card.GetPlayerIdentifier());
			}

			for (auto death_item : ordered_deaths) death_item.second();

			return true;
		}

		inline bool Resolver::SetResult(FlowContext & flow_context, Result result)
		{
			assert(result != kResultNotDetermined);
			flow_context.SetResult(result);
			return false;
		}

		inline void Resolver::UpdateAura(state::State & state, FlowContext & flow_context)
		{
			state.GetAuraManager().ForEachAura([&state, &flow_context](FlowControl::aura::Handler & handler)
			{
				bool aura_valid = handler.Update(state, flow_context);
				if (aura_valid) return true;

				assert(handler.NoAppliedEnchantment());
				return false;
			});
		}

		inline void Resolver::UpdateEnchantments(state::State & state, FlowContext & flow_context)
		{
			UpdateEnchantments(state, flow_context, state::PlayerIdentifier::First());
			UpdateEnchantments(state, flow_context, state::PlayerIdentifier::Second());
		}

		inline void Resolver::UpdateEnchantments(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player)
		{
			state::CardRef weapon_ref = state.GetBoard().Get(player).GetWeaponRef();

			Manipulate(state, flow_context).Hero(player).Enchant().Update();

			if (weapon_ref.IsValid()) {
				Manipulate(state, flow_context).Weapon(weapon_ref).Enchant().Update();
			}

			std::vector<state::CardRef> minions_refs; // need to cache first, since minion might be removed from the container
			minions_refs = state.GetBoard().Get(player).minions_.GetAll();
			for (state::CardRef minion_ref : minions_refs) {
				Manipulate(state, flow_context).OnBoardMinion(minion_ref).Enchant().Update();
			}
		}
	}
}