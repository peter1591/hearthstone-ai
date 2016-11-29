#pragma once

#include <map>
#include "State/Types.h"
#include "State/State.h"
#include "FlowControl/Result.h"
#include "FlowControl/Manipulate.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace Helpers
	{
		class EntityDeathHandler
		{
		public:
			EntityDeathHandler(state::State & state, FlowContext & flow_context) : state_(state), flow_context_(flow_context) {}

			Result ProcessDeath()
			{
				for (const auto& item : flow_context_.dead_entity_hints_)
				{
					state::CardRef ref = item.second;
					const state::Cards::Card & card = state_.mgr.Get(ref);

					if (card.GetZone() != state::kCardZonePlay) continue;
					if (card.GetHP() > 0) continue;
					// TODO: check if it's pending destory

					if (card.GetCardType() == state::kCardTypeHero) {
						return ProcessHeroDeath(card, ref);
					}

					Manipulate(state_, flow_context_).Card(ref).Deathrattles()
						.TriggerAll()
						.Clear();

					Manipulate(state_, flow_context_).Card(ref).Zone().ChangeTo<state::kCardZoneGraveyard>(card.GetPlayerIdentifier());
				}
				flow_context_.dead_entity_hints_.clear();

				return kResultNotDetermined;
			}

		private:
			Result ProcessHeroDeath(const state::Cards::Card & card, state::CardRef ref)
			{
				if (card.GetPlayerIdentifier() == state::kPlayerFirst) {
					state::CardRef another_hero = state_.board.Get(state::kPlayerSecond).hero_ref_;
					if (state_.mgr.Get(another_hero).GetHP() <= 0) return kResultDraw;
					else return kResultSecondPlayerWin;
				}
				else {
					state::CardRef another_hero = state_.board.Get(state::kPlayerFirst).hero_ref_;
					if (state_.mgr.Get(another_hero).GetHP() <= 0) return kResultDraw;
					else return kResultFirstPlayerWin;
				}
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}