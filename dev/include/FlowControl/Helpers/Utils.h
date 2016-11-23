#pragma once

namespace FlowControl
{
	namespace Helpers
	{
		class Utils
		{
		public:
			static Result CheckWinLoss(State::State & state)
			{
				const Entity::Card & first_hero = state.mgr.Get(state.players.Get(State::kPlayerFirst).hero_ref_);
				bool first_died = first_hero.GetHP() < 0;
				const Entity::Card & second_hero = state.mgr.Get(state.players.Get(State::kPlayerSecond).hero_ref_);
				bool second_died = second_hero.GetHP() < 0;

				if (first_died) {
					if (second_died) return kResultDraw;
					else return kResultSecondPlayerWin;
				}
				else {
					if (second_died) return kResultFirstPlayerWin;
					else return kResultNotDetermined;
				}
			}
		};
	}
}