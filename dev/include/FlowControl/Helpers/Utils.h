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
				bool first_died = state.players.Get(State::kPlayerFirst).state_.GetHP() < 0;
				bool second_died = state.players.Get(State::kPlayerSecond).state_.GetHP() < 0;

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