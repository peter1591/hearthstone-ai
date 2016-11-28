#pragma once

#include <map>
#include "State/Types.h"
#include "State/State.h"
#include "FlowControl/Result.h"

namespace FlowControl
{
	namespace Helpers
	{
		class EntityDeathHandler
		{
		private:
			typedef std::multimap<int, state::CardRef> ContainerType;

		public:
			EntityDeathHandler(state::State & state) : state_(state) {}

			void Add(state::CardRef ref)
			{
				int play_order = state_.mgr.Get(ref).GetPlayOrder();

				entities_.insert(std::make_pair(play_order, ref));
			}

			Result ProcessDeath()
			{
				for (const auto& item : entities_)
				{
					state::CardRef ref = item.second;
					const state::Cards::Card & card = state_.mgr.Get(ref);

					if (card.GetZone() != state::kCardZonePlay) continue;
					if (card.GetHP() > 0) continue;
					// TODO: check if it's pending destory

					if (card.GetCardType() == state::kCardTypeHero) {
						return ProcessHeroDeath(card, ref);
					}

					// TODO: trigger deathrattle
					Manipulate(state_).Card(ref).Zone().ChangeTo<state::kCardZoneGraveyard>(card.GetPlayerIdentifier());
				}
				entities_.clear();

				return kResultNotDetermined;
			}

		private:
			Result ProcessHeroDeath(const state::Cards::Card & card, state::CardRef ref)
			{
				state::CardRef another_hero;
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
			ContainerType entities_;
		};
	}
}