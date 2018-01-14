#pragma once

namespace alphazero
{
	namespace evaluation
	{
		struct CompetitionResult
		{
			CompetitionResult() :
				total_games(0),
				competitor_wins(0)
			{}

			void Clear() {
				total_games = 0;
				competitor_wins = 0;
			}

			CompetitionResult & operator+=(CompetitionResult const& rhs) {
				total_games += rhs.total_games;
				competitor_wins += rhs.competitor_wins;
				return *this;
			}

			int total_games;
			int competitor_wins;
		};
	}
}