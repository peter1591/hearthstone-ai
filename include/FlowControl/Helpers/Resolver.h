#pragma once

#include <unordered_set>
#include <map>
#include "State/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Helpers
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
				UpdateAura();
				UpdateEnchantments();

				bool any_death = false;
				while (true) {
					CreateDeaths();
					if (deaths_.empty()) break;

					any_death = true;
					if (!RemoveDeaths()) return false;
				}

				if (any_death) {
					UpdateAura();
					UpdateEnchantments();
				}

				return true;
			}

		private:
			void CreateDeaths()
			{
				deaths_.clear();

				if (flow_context_.destroyed_weapon_.IsValid()) {
					state::Cards::Card const& card = state_.mgr.Get(flow_context_.destroyed_weapon_);
					flow_context_.dead_entity_hints_.insert(
						std::make_pair(card.GetPlayOrder(), flow_context_.destroyed_weapon_));
					flow_context_.destroyed_weapon_.Invalidate();
				}

				for (const auto& item : flow_context_.dead_entity_hints_)
				{
					state::CardRef ref = item.second;
					const state::Cards::Card & card = state_.mgr.Get(ref);

					if (card.GetZone() != state::kCardZonePlay) continue;
					if (card.GetHP() > 0) continue;
					// TODO: check if it's pending destory

					deaths_.insert(ref);
				}

				flow_context_.dead_entity_hints_.clear();
			}

			bool RemoveDeaths()
			{
				auto first_it = deaths_.find(state_.board.Get(state::kPlayerFirst).hero_ref_);
				bool first_dead = first_it != deaths_.end();

				auto second_it = deaths_.find(state_.board.Get(state::kPlayerSecond).hero_ref_);
				bool second_dead = second_it != deaths_.end();

				if (first_dead && second_dead) return SetResult(kResultDraw);
				if (first_dead) return SetResult(kResultSecondPlayerWin);
				if (second_dead) return SetResult(kResultFirstPlayerWin);

				// process deaths by order of play
				std::multimap<int, std::pair<state::CardRef, state::Cards::Card const*>> ordered_deaths;

				for (auto ref : deaths_)
				{
					const state::Cards::Card & card = state_.mgr.Get(ref);
					ordered_deaths.insert(std::make_pair(card.GetPlayOrder(), std::make_pair(ref, &card)));
				}

				for (auto death_item: ordered_deaths)
				{
					state::CardRef & ref = death_item.second.first;
					state::Cards::Card const& card = *death_item.second.second;

					Manipulate(state_, flow_context_).Card(ref).Deathrattles()
						.TriggerAll()
						.Clear();

					Manipulate(state_, flow_context_).Card(ref).Zone().ChangeTo<state::kCardZoneGraveyard>(card.GetPlayerIdentifier());
				}

				return true;
			}

			bool SetResult(Result result)
			{
				assert(result != kResultNotDetermined);
				flow_context_.result_ = result;
				return false;
			}

		private:
			void UpdateAura()
			{
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::UpdateAura>(
					state::Events::EventTypes::UpdateAura::Context{state_, flow_context_}
				);
			}

			void UpdateEnchantments()
			{
				UpdateEnchantments(state::kPlayerFirst);
				UpdateEnchantments(state::kPlayerSecond);
			}

			void UpdateEnchantments(state::PlayerIdentifier player)
			{
				state::CardRef hero_ref = state_.board.Get(player).hero_ref_;
				state::CardRef weapon_ref = state_.mgr.Get(hero_ref).GetRawData().weapon_ref;

				Manipulate(state_, flow_context_).Hero(player).Enchant().Update();

				if (weapon_ref.IsValid()) {
					Manipulate(state_, flow_context_).Weapon(weapon_ref).Enchant().Update();
				}

				for (state::CardRef minion_ref : state_.board.Get(player).minions_.Get()) {
					Manipulate(state_, flow_context_).Minion(minion_ref).Enchant().Update();
				}
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
			std::unordered_set<state::CardRef> deaths_;
		};
	}
}