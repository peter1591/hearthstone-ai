#pragma once

#include <unordered_set>
#include <map>
#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace detail
	{
		class Resolver
		{
		public:
			Resolver(state::State & state, FlowContext & flow_context)
				: state_(state), flow_context_(flow_context)
			{
			}

			bool Resolve()
			{
				while (true) {
					auto first_minions_change_id = state_.GetBoard().GetFirst().minions_.GetChangeId();
					auto second_minions_change_id = state_.GetBoard().GetSecond().minions_.GetChangeId();

					UpdateAura(); // update aura first, since aura will add/remove enchantments on others
					UpdateEnchantments();

					do {
						CreateDeaths();
						if (!RemoveDeaths()) return false;
					} while (!deaths_.empty());

					bool done = true;

					// If any minions change side (due to enchantment), re-resolve again
					auto new_first_minions_change_id = state_.GetBoard().GetFirst().minions_.GetChangeId();
					auto new_second_minions_change_id = state_.GetBoard().GetSecond().minions_.GetChangeId();
					if (new_first_minions_change_id != first_minions_change_id ||
						new_second_minions_change_id != second_minions_change_id) {
						done = false;
					}
					if (!done) continue;

					break;
				}

				return true;
			}

		private:
			void CreateDeaths()
			{
				deaths_.clear();

				if (flow_context_.GetDestroyedWeapon().IsValid()) {
					flow_context_.AddDeadEntryHint(state_, flow_context_.GetDestroyedWeapon());
					flow_context_.ClearDestroyedWeapon();
				}

				flow_context_.ForEachDeadEntryHint([this](auto const& item) {
					state::CardRef ref = item.second;
					const state::Cards::Card & card = state_.GetCardsManager().Get(ref);

					if (card.GetZone() != state::kCardZonePlay) return;
					if (!card.GetRawData().pending_destroy) {
						if (card.GetHP() > 0) return;
					}

					deaths_.insert(ref);
				});

				flow_context_.ClearDeadEntryHint();
			}

			bool RemoveDeaths()
			{
				auto first_it = deaths_.find(state_.GetBoard().GetFirst().GetHeroRef());
				bool first_dead = first_it != deaths_.end();

				auto second_it = deaths_.find(state_.GetBoard().GetSecond().GetHeroRef());
				bool second_dead = second_it != deaths_.end();

				if (first_dead && second_dead) return SetResult(kResultDraw);
				if (first_dead) return SetResult(kResultSecondPlayerWin);
				if (second_dead) return SetResult(kResultFirstPlayerWin);

				// process deaths by order of play
				std::multimap<int, std::function<void()>> ordered_deaths;

				for (auto ref : deaths_) {
					state::Cards::Card & card = state_.GetMutableCard(ref);

					state::PlayerIdentifier player = card.GetPlayerIdentifier();
					state::CardZone zone = card.GetZone();
					int zone_pos = card.GetZonePosition();
					int attack = card.GetAttack();

					ordered_deaths.insert(std::make_pair(card.GetPlayOrder(),
						[ref, player, zone, zone_pos, attack, this]() {
						state_.GetMutableCard(ref).GetMutableDeathrattleHandler().TriggerAll(
							FlowControl::deathrattle::context::Deathrattle{
							FlowControl::Manipulate(state_, flow_context_), ref, player, zone, zone_pos, attack });

						if (state_.GetCard(ref).GetCardType() == state::kCardTypeMinion) {
							state_.TriggerEvent<state::Events::EventTypes::AfterMinionDied>(state::Events::EventTypes::AfterMinionDied::Context{
								FlowControl::Manipulate(state_, flow_context_), ref, player
							});
						}
					}));
				}

				for (auto ref : deaths_) {
					state::Cards::Card & card = state_.GetMutableCard(ref);
					state_.GetZoneChanger(ref)
						.ChangeTo<state::kCardZoneGraveyard>(card.GetPlayerIdentifier());
				}

				for (auto death_item: ordered_deaths) death_item.second();

				return true;
			}

			bool SetResult(Result result)
			{
				assert(result != kResultNotDetermined);
				flow_context_.SetResult(result);
				return false;
			}

		private:
			void UpdateAura()
			{
				state_.GetAuraManager().ForEachAura([this](FlowControl::aura::Handler & handler)
				{
					bool aura_valid = handler.Update(state_, flow_context_);
					if (aura_valid) return true;

					assert(handler.NoAppliedEnchantment());
					return false;
				});
			}

			void UpdateEnchantments()
			{
				UpdateEnchantments(state::PlayerIdentifier::First());
				UpdateEnchantments(state::PlayerIdentifier::Second());
			}

			void UpdateEnchantments(state::PlayerIdentifier player)
			{
				state::CardRef hero_ref = state_.GetBoard().Get(player).GetHeroRef();
				state::CardRef weapon_ref = state_.GetBoard().Get(player).GetWeaponRef();

				Manipulate(state_, flow_context_).Hero(player).Enchant().Update();

				if (weapon_ref.IsValid()) {
					Manipulate(state_, flow_context_).Weapon(weapon_ref).Enchant().Update();
				}

				std::vector<state::CardRef> minions_refs; // need to cache first, since minion might be removed from the container
				minions_refs = state_.GetBoard().Get(player).minions_.GetAll();
				for (state::CardRef minion_ref : minions_refs) {
					Manipulate(state_, flow_context_).OnBoardMinion(minion_ref).Enchant().Update();
				}
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
			std::unordered_set<state::CardRef> deaths_;
		};
	}
}